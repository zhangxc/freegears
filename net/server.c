#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

int port = 8000;

int main(void)
{
	struct sockaddr_in sin;
	struct sockaddr_in pin;
	int sock_descriptor;
	int tmp_sock_descriptor;
	int address_size;
	char buf[16384];
	int i, len;

	sock_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_descriptor == -1) {
		perror("Socket ");
		exit(1);
	}

	bzero(&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);
	
	if (bind(sock_descriptor, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
		perror("Bind ");
		exit(1);
	}

	if (listen(sock_descriptor, 20) == -1) {
		perror("Listen ");
		exit(1);
	}

	printf("Waiting for connections ...\n");
	
	while (1) {
		tmp_sock_descriptor = accept(sock_descriptor, (struct sockaddr *)&pin, &address_size);
		if (tmp_sock_descriptor == -1) {
			perror("Accept ");
			exit(1);
		}

		if (recv(tmp_sock_descriptor, buf, 16384, 0) == -1) {
			perror("Recv ");
			exit(1);
		}

		printf("received from client: %s\n", buf);

		len = strlen(buf);
		for (i = 0; i < len; i++)
			buf[i] = toupper(buf[i]);
		
		if (send(tmp_sock_descriptor, buf, len, 0) == -1) {
			perror("Send ");
			exit(1);
		}
		close(tmp_sock_descriptor);
	}
}
