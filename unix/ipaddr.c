#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/if_arp.h>


int main(int argc, char **argv) 
{
	int sockfd;
	struct ifreq buf[16];
	struct ifconf ifc;
	int i;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);	/* create a UDP socket */
	if (sockfd < 0) {
		perror("socket");
		exit(-1);
	}

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = (caddr_t)buf;
	
	/* SIOCGIFCONF, struct ifconf *. */
	if (!ioctl(sockfd, SIOCGIFCONF, (char *)&ifc))
		i = ifc.ifc_len / sizeof(struct ifreq);
	else {
		perror("ioctl SIOCGIFCONF");
		goto FAIL;
	}
		
	while (i-- > 0) {
		if (!strcmp(buf[i].ifr_name, "lo"))
			continue;

		/* SIOCGIFADDR, struct ifreq *. */
		if (!ioctl(sockfd, SIOCGIFADDR, (char *)&buf[i])) {
			printf("interface %d: %s\tIP %s\t", 
			       i, buf[i].ifr_name, 
		inet_ntoa(((struct sockaddr_in *)(&buf[i].ifr_addr))->sin_addr));
		} else {
			perror("ioctl SIOCGIFADDR");
			goto FAIL;
		}

		/*Get HW ADDRESS of the net card */
		if (!(ioctl(sockfd, SIOCGIFHWADDR, (char *) &buf[i]))) {
			printf("HW %02x:%02x:%02x:%02x:%02x:%02x",
			       (unsigned char) buf[i].ifr_hwaddr.sa_data[0],
			       (unsigned char) buf[i].ifr_hwaddr.sa_data[1],
			       (unsigned char) buf[i].ifr_hwaddr.sa_data[2],
			       (unsigned char) buf[i].ifr_hwaddr.sa_data[3],
			       (unsigned char) buf[i].ifr_hwaddr.sa_data[4],
			       (unsigned char) buf[i].ifr_hwaddr.sa_data[5]);

		} else {
			perror("ioctl SIOCGIFHWADDR");
		}
	}
FAIL:
	puts("");
	close(sockfd);
	return 0;
}
