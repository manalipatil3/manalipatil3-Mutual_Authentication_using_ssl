#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

int InitSocket(int port) {
    int sd;
    struct sockaddr_in addr;

    sd = socket(PF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        perror("Bind failed");
        close(sd);
        exit(EXIT_FAILURE);
    }

    if (listen(sd, 10) != 0) {
        perror("Listen failed");
        close(sd);
        exit(EXIT_FAILURE);
    }

    return sd;
}

SSL_CTX* InitSSLContext(const char* mode, const char* keyfile, const char* certfile) {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    // Initialize OpenSSL
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();

    method = TLS_server_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    
    if (certfile!=NULL && SSL_CTX_use_certificate_file(ctx, certfile, SSL_FILETYPE_PEM) <= 0) {
        perror("Client Certificate Verification Failed\n");
        exit(EXIT_FAILURE); 
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, keyfile, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE); 
    }

    if (strcmp(mode, "mutual") == 0) {
        SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
        SSL_CTX_load_verify_locations(ctx,"ca.pem",NULL);
    }
    return ctx;
}

void Process(SSL *ssl) {
    char buffer[1024];
    int num;

    while (1) {
        int bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1);
        if (bytes <= 0) {
            int err = SSL_get_error(ssl, bytes);
            if (err == SSL_ERROR_ZERO_RETURN || err == SSL_ERROR_SYSCALL) {
                printf("Client closed the connection\n");
            } else {
                ERR_print_errors_fp(stderr);
            }
            break;
        }

        buffer[bytes] = '\0';
        printf("Received: %s\n", buffer);
        num = atoi(buffer);
        num++;

        sprintf(buffer, "%d", num);
        if (SSL_write(ssl, buffer, strlen(buffer)) <= 0) {
            ERR_print_errors_fp(stderr);
            break;
        }
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
}

int main(int argc, char *argv[]) {
    if (argc > 5 || argc<4) {
        fprintf(stderr, "Usage: %s <port> <mode: oneway|mutual> <server.key> <server.crt>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    char *mode = argv[2];
    char *keyfile = argv[3];
    char *certfile = NULL;
    if(argc==5){
        certfile=argv[4];
    }

    SSL_CTX *ctx = InitSSLContext(mode, keyfile, certfile);
    int server_sd = InitSocket(port);

    printf("Listening on port %d\n", port);

    while (1) {
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        int client_sd = accept(server_sd, (struct sockaddr *)&addr, &len);

        if (client_sd < 0) {
            perror("Accept failed");
            continue;
        }

        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client_sd);

        if (SSL_accept(ssl) <= 0) {
            if (SSL_get_verify_result(ssl) != X509_V_OK) {
                perror("Client certificate verification failed\n");
                SSL_free(ssl);
                close(server_sd);
                SSL_CTX_free(ctx);
                exit(EXIT_FAILURE);
            }
            if(strcmp(mode,"mutual")==0)
            perror("Client Certificate Missing\n");
            SSL_free(ssl);
            close(client_sd);
            continue;
        }

        printf("Connection from %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
        Process(ssl);
    }

    close(server_sd);
    SSL_CTX_free(ctx);
    EVP_cleanup();
    return 0;
}
