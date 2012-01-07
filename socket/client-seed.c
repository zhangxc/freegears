#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

char * host_name = "127.0.0.1";

int port = 8000;

int main(int argc, char *argv[]) 
{
	int i;
	char buf[8192];
	char str[256];
	int socket_descriptor;
	struct sockaddr_in pin;
	struct hostent *server_host_name;

	if ((server_host_name = gethostbyname(host_name)) == 0) {
		perror("Error resolving local host\n");
		exit(1);
	}

	bzero(&pin, sizeof(pin));
	pin.sin_family = AF_INET;
	pin.sin_addr.s_addr = htonl(INADDR_ANY);
	pin.sin_addr.s_addr = ((struct in_addr *)(server_host_name->h_addr))->s_addr;
	pin.sin_port = htons(port);

	if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket ");
		exit(1);
	}

	if (connect(socket_descriptor, (void *)&pin, sizeof(pin)) == -1) {
		perror("Connect ");
		exit(1);
	}

	printf("Sending message to server ...\n");

	for (i = 0; i < 100; i++){
		sprintf(str, "Seed %d", i);
		if (send(socket_descriptor, str, 16384, 0) == -1) {
			perror("Send ");
//			exit(1);
		}
	}

/*
	printf(".. sent message .. wait for response ...\n");
	if (recv(socket_descriptor, buf, 8192, 0) == -1) {
		perror("Recv ");
		exit(1);
	}
*/

	printf("\nResponse from server: \n\n\t%s\n", buf);
	close(socket_descriptor);
	return 0;
}
