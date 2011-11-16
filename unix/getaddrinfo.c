#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(void)
{
       struct addrinfo *infop = NULL, hint;

       memset(&hint, 0, sizeof(hint));
       hint.ai_family = AF_INET;
       hint.ai_socktype = SOCK_STREAM;
       int ret = getaddrinfo("google.com", "80", &hint, &infop);

       if(ret)
               printf("%s\n", gai_strerror(ret));

       for( ; infop != NULL; infop = infop->ai_next){
               struct sockaddr_in *sa = (struct sockaddr_in *)infop->ai_addr;
               printf("%s port: %d protocol: %d\n", 
		      inet_ntoa(sa->sin_addr), ntohs(sa->sin_port), 
		      infop->ai_protocol);
       }

       return 0;
}
