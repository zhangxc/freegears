#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>

#define MAXLINE 1024

int main(void)
{
	int maxfdp1, fd0;
	int n;
	fd_set rset;
	char buf[MAXLINE];

	fd0 = fileno(stdin);
	FD_ZERO(&rset);
	while(1) {
		FD_SET(fd0, &rset);
		maxfdp1 = fd0 + 1;
		select(maxfdp1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(fd0, &rset)) {
			n = read(fd0, buf, MAXLINE);
			printf("read stdin: %d\n", n);
		}
	}
}
