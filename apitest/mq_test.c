#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <string.h>
#include <stdio.h>


#define MSGSIZE 128
struct msgbuf {
	long    mtype;
	char    mtext[MSGSIZE];
};


int main(void)
{
	int mq;
	int buflen;
	key_t key;
	struct msgbuf sbuf;

	key = 4466;
	if ((mq = msgget(key, IPC_CREAT | 0666)) < 0) {
		perror("msgget");
		return 1;
	}

	sbuf.mtype = 1;
	snprintf(sbuf.mtext, MSGSIZE, "message = %d", mq);
	buflen = strlen(sbuf.mtext) + 1;

	if (msgsnd(mq, &sbuf, buflen, IPC_NOWAIT) == -1) {
		perror("msgsnd");
		return 2;
	}

	memset(&sbuf, 0, sizeof(struct msgbuf));
	if (msgrcv(mq, &sbuf, MSGSIZE, 1, 0) < 0) {
		perror("msgrcv");
		return 2;
	}

	printf("msgget(%ld): %s\n", sbuf.mtype, sbuf.mtext);
	return 0;
}
