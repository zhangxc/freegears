#include <stdio.h>
#include <unistd.h>
#include <termios.h>

int main(void)
{
	struct termios old_tio, new_tio;
	unsigned char c;

	tcgetattr(STDIN_FILENO,&old_tio);

	new_tio = old_tio;

	/* non-canonical mode and without echoes */
	new_tio.c_lflag &= (~ICANON & ~ECHO);

	tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);
	getchar();
	tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);

	return 0;
}
