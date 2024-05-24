#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>

#define HOSTNAME "localhost"
#define CA_CERT_FILE "ca.pem"

SSL_CTX* InitSSLContext(const char* client_key, const char* client_cert) {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    SSL_library_init();
    OpenSSL_add_all_algorithms();  
    SSL_load_error_strings();     
    method = TLS_client_method(); 
    ctx = SSL_CTX_new(method);    
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        abort();
    }

    if (!SSL_CTX_load_verify_locations(ctx, CA_CERT_FILE, NULL)) {
        fprintf(stderr, "Error loading CA certificate file %s\n", CA_CERT_FILE);
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
    SSL_CTX_set_verify_depth(ctx, 4);

    if (client_cert && client_key) {
        if (SSL_CTX_use_certificate_file(ctx, client_cert, SSL_FILETYPE_PEM) <= 0 ||
            SSL_CTX_use_PrivateKey_file(ctx, client_key, SSL_FILETYPE_PEM) <= 0 ||
            !SSL_CTX_check_private_key(ctx)) {
            fprintf(stderr, "Error setting up SSL/TLS client authentication.\n");
            ERR_print_errors_fp(stderr);
            exit(EXIT_FAILURE);
        }
    }

    return ctx;
}

int InitSocket(char *hostname, int port) {
    int sd;
    struct hostent *host;
    struct sockaddr_in addr;

    host = gethostbyname(hostname);
    if (!host) {
        perror("Error resolving hostname");
        exit(EXIT_FAILURE);
    }

    sd = socket(PF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        perror("Cannot create socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = *(struct in_addr*)host->h_addr;

    if (connect(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        perror("Connect failed");
        close(sd);
        exit(EXIT_FAILURE);
    }
    return sd;
}

void Process(SSL *ssl) {
    char buffer[21];
    char num[20];

    printf("Connection established. Please input numbers.\n");
    while (1) {
        printf("Input a number: ");
        if (!fgets(num, sizeof(num), stdin)) {
            if (feof(stdin)) {
                printf("\nEOF received. Exiting...\n");
            } else {
                perror("Error reading input");
            }
            break;
        }

        num[strcspn(num, "\n")] = 0;  // Strip newline

        if (strcmp(num, "exit") == 0) {
            printf("Exit command received. Closing connection.\n");
            break;
        }

        if (SSL_write(ssl, num, strlen(num)) <= 0) {
            ERR_print_errors_fp(stderr);
            break;
        }

        int bytes = SSL_read(ssl, buffer, sizeof(buffer)-1);
        if (bytes <= 0) {
            ERR_print_errors_fp(stderr);
            break;
        }
        buffer[bytes] = '\0';
        printf("Received from server: %s\n", buffer);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 4) {
        fprintf(stderr, "Usage: %s <port> [client.crt client.key]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    SSL_CTX *ctx = (argc == 4) ? InitSSLContext(argv[2], argv[3]) : InitSSLContext(NULL, NULL);

    int server_sd = InitSocket(HOSTNAME, port);
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, server_sd);

    
    if (SSL_connect(ssl) != 1) {
        if (SSL_get_verify_result(ssl) != X509_V_OK) {
        perror("Server certificate verification failed\n");
        SSL_free(ssl);
        close(server_sd);
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }
        perror("Server Certificate Missing\n");
        SSL_free(ssl);
        close(server_sd);
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    

    Process(ssl);

    SSL_free(ssl);
    close(server_sd);
    SSL_CTX_free(ctx);
    return 0;
}
