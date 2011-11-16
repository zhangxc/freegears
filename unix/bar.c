/* bar.c
 */
#include <stdio.h>

void bar();

int main() 
{
	printf("PING"); 
	bar(); 
	printf("NETSTAT"); 
	bar(); 
	return(0);
}

void bar()
{ 
	int i,j; 

	printf("\n"); 
	for(i = 0;i <= 10; i++) {
		printf("["); 

		for (j = 0; j < i; j++)
			printf(">"); 
		for (; j < 10; j++) 
			printf(" "); 

		printf("]"); 
		printf("%3d%%", i*10); 
		if(i == 10)
			printf(" [COMPLETED]"); 

		printf("\r");
		fflush(stdout);
		sleep(1); 
	} 
	printf("\n"); 
}

