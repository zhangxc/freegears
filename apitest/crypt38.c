#include <stdio.h>
#include <sys/socket.h>
#include <linux/if_alg.h>

#ifndef AF_ALG
#define AF_ALG 38
#define SOL_ALG 279
#endif

int main(void)
{
	int opfd;
	int tfmfd;

	/* algorithm socket 
	 * here comes a new sockaddr with a new sock family
	 */
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "hash",
		.salg_name = "sha1"
	};
	char buf[20];
	int i;

	/* using SOCK_SEQPACKET, socket + bind + accept */
	tfmfd = socket(AF_ALG, SOCK_SEQPACKET, 0); 
	bind(tfmfd, (struct sockaddr *)&sa, sizeof(sa));
	opfd = accept(tfmfd, NULL, 0);

	/* write and read */
	write(opfd, "abc", 3);
	read(opfd, buf, 20);

	for (i = 0; i < 20; i++) {
		printf("%02x", (unsigned char)buf[i]);
	}
	printf("\n");

	close(opfd);
	close(tfmfd);

	return 0;
}
