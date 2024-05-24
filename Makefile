all: server client
server: server.c
	gcc -Wall server.c -o server -lssl -lcrypto

client: client.c
	gcc -Wall client.c -o client -lssl -lcrypto
clean:
	rm server client
