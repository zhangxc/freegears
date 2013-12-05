/*
 * Event loop
 * Copyright 2002-2003, Jouni Malinen <jkmaline@cc.hut.fi>
 * Copyright 2004, Instant802 Networks, Inc.
 * All Rights Reserved.
 */

#include "eloop.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static struct eloop_data eloop;

static inline int eloop_gettime(struct timeval *tv, ...)
{
	struct timespec res;

	if (!tv || clock_gettime(CLOCK_MONOTONIC, &res)) {
		return -1;
	}
	tv->tv_sec = res.tv_sec;
	tv->tv_usec = res.tv_nsec / 1000;
	return 0;
}

void eloop_init(void *user_data)
{
	memset(&eloop, 0, sizeof(eloop));
	eloop.user_data = user_data;
}


int eloop_register_read_sock(int sock,
			     void (*handler)(int sock, void *eloop_ctx,
					     void *sock_ctx),
			     void *eloop_data, void *user_data)
{
	struct eloop_sock *tmp;

	tmp = (struct eloop_sock *)
		realloc(eloop.readers,
			(eloop.reader_count + 1) * sizeof(struct eloop_sock));
	if (tmp == NULL)
		return -1;

	tmp[eloop.reader_count].sock = sock;
	tmp[eloop.reader_count].eloop_data = eloop_data;
	tmp[eloop.reader_count].user_data = user_data;
	tmp[eloop.reader_count].handler = handler;
	eloop.reader_count++;
	eloop.readers = tmp;
	if (sock > eloop.max_sock)
		eloop.max_sock = sock;

	return 0;
}


int eloop_unregister_read_sock(int sock)
{
	int i;

	for (i = 0; i < eloop.reader_count; i++) {
		if (eloop.readers[i].sock == sock)
			break;
	}

	if (i >= eloop.reader_count)
		return -1;

	if (i + 1 < eloop.reader_count)
		memmove(&eloop.readers[i], &eloop.readers[i + 1],
			(eloop.reader_count - i - 1) *
			sizeof(struct eloop_sock));
	eloop.reader_count--;

	/* max_sock for select need not be exact, so no need to update it */
	/* don't bother reallocating block, since this area is quite small and
	 * next registration will realloc anyway */

	return 0;
}


int eloop_register_timeout(unsigned int secs, unsigned int usecs,
			   void (*handler)(void *eloop_ctx, void *timeout_ctx),
			   void *eloop_data, void *user_data)
{
	struct eloop_timeout *timeout, *tmp, *prev;

	timeout = (struct eloop_timeout *) malloc(sizeof(*timeout));
	if (timeout == NULL)
		return -1;
	eloop_gettime(&timeout->time, NULL);
	timeout->time.tv_sec += secs;
	timeout->time.tv_usec += usecs;
	while (timeout->time.tv_usec >= 1000000) {
		timeout->time.tv_sec++;
		timeout->time.tv_usec -= 1000000;
	}
	timeout->eloop_data = eloop_data;
	timeout->user_data = user_data;
	timeout->handler = handler;
	timeout->next = NULL;

	if (eloop.timeout == NULL) {
		eloop.timeout = timeout;
		return 0;
	}

	prev = NULL;
	tmp = eloop.timeout;
	while (tmp != NULL) {
		if (timercmp(&timeout->time, &tmp->time, <))
			break;
		prev = tmp;
		tmp = tmp->next;
	}

	if (prev == NULL) {
		timeout->next = eloop.timeout;
		eloop.timeout = timeout;
	} else {
		timeout->next = prev->next;
		prev->next = timeout;
	}

	return 0;
}


int eloop_cancel_timeout(void (*handler)(void *eloop_ctx, void *sock_ctx),
			 void *eloop_data, void *user_data)
{
	struct eloop_timeout *timeout, *prev, *next;
	int removed = 0;

	prev = NULL;
	timeout = eloop.timeout;
	while (timeout != NULL) {
		next = timeout->next;

		if (timeout->handler == handler &&
		    (timeout->eloop_data == eloop_data ||
		     eloop_data == ELOOP_ALL_CTX) &&
		    (timeout->user_data == user_data ||
		     user_data == ELOOP_ALL_CTX)) {
			if (prev == NULL)
				eloop.timeout = next;
			else
				prev->next = next;
			free(timeout);
			removed++;
		} else
			prev = timeout;

		timeout = next;
	}

	return removed;
}


static void eloop_handle_signal(int sig)
{
	int i;

	eloop.signaled++;
	for (i = 0; i < eloop.signal_count; i++) {
		if (eloop.signals[i].sig == sig) {
			eloop.signals[i].signaled++;
			break;
		}
	}
}


static void eloop_process_pending_signals(void)
{
	int i;

	if (eloop.signaled == 0)
		return;
	eloop.signaled = 0;

	for (i = 0; i < eloop.signal_count; i++) {
		if (eloop.signals[i].signaled) {
			eloop.signals[i].signaled = 0;
			eloop.signals[i].handler(eloop.signals[i].sig,
						 eloop.user_data,
						 eloop.signals[i].user_data);
		}
	}
}


int eloop_register_signal(int sig,
			  void (*handler)(int sig, void *eloop_ctx,
					  void *signal_ctx),
			  void *user_data)
{
	struct eloop_signal *tmp;

	tmp = (struct eloop_signal *)
		realloc(eloop.signals,
			(eloop.signal_count + 1) *
			sizeof(struct eloop_signal));
	if (tmp == NULL)
		return -1;

	tmp[eloop.signal_count].sig = sig;
	tmp[eloop.signal_count].user_data = user_data;
	tmp[eloop.signal_count].handler = handler;
	tmp[eloop.signal_count].signaled = 0;
	eloop.signal_count++;
	eloop.signals = tmp;
	signal(sig, eloop_handle_signal);

	return 0;
}

void eloop_run(void)
{
	fd_set rfds;
	int i, res;
	struct timeval tv, now;

	while (!eloop.terminate &&
		(eloop.timeout || eloop.reader_count > 0 || eloop.agent != NULL)) {
		if (eloop.timeout) {
			eloop_gettime(&now, NULL);
#if 1
			/* Workaround for faulty kernels - cannot allow
			 * tv_usec to be >= 1000000 because otherwise tv_usec
			 * for select might be invalid and select() would fail
			 * with "Invalid argument". This error has been noticed
			 * at least with MIPS kernel and idt438. */
			if (now.tv_usec < 0 || now.tv_usec >= 1000000) {
				printf("ERROR: eloop_gettime returned invalid "
				       "time: tv_sec=%d tv_usec=%d\n",
				       (int) now.tv_sec, (int) now.tv_usec);
				now.tv_usec = 999999;
			}
#endif
			if (timercmp(&now, &eloop.timeout->time, <))
				timersub(&eloop.timeout->time, &now, &tv);
			else
				tv.tv_sec = tv.tv_usec = 0;
#if 0
			printf("next timeout in %lu.%06lu sec\n",
			       tv.tv_sec, tv.tv_usec);
#endif
		}

		FD_ZERO(&rfds);
		for (i = 0; i < eloop.reader_count; i++)
			FD_SET(eloop.readers[i].sock, &rfds);

		int max_fd = eloop.max_sock + 1;
		if (NULL != eloop.agent) {
			max_fd = (max_fd > eloop.max_sock ? (max_fd+1) : (eloop.max_sock+1));
		}
		res = select(max_fd, &rfds, NULL, NULL,
                eloop.timeout ? &tv : NULL);
		if (res < 0 && errno != EINTR) {
			perror("select");
			return;
		}

		eloop_process_pending_signals();

		/* check if some registered timeouts have occurred */
		if (eloop.timeout) {
			struct eloop_timeout *tmp;

			eloop_gettime(&now, NULL);
			if (!timercmp(&now, &eloop.timeout->time, <)) {
				tmp = eloop.timeout;
				eloop.timeout = eloop.timeout->next;
				tmp->handler(tmp->eloop_data,
					     tmp->user_data);
				free(tmp);
			}

		}

		if (res <= 0)
			continue;


		for (i = 0; i < eloop.reader_count; i++) {
			if (FD_ISSET(eloop.readers[i].sock, &rfds)) {
				eloop.readers[i].handler(
					eloop.readers[i].sock,
					eloop.readers[i].eloop_data,
					eloop.readers[i].user_data);
			}
		}
	}
}

/* eloop_run thread running version */
void *eloop_run_thread(void *arg)
{
    fd_set rfds;
    int max_fd, res;
    struct timeval tv;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while (eloop.agent != NULL) {
        FD_ZERO(&rfds);

        max_fd = max_fd + 1;

        tv.tv_sec = tv.tv_usec = 0;

        res = select(max_fd, &rfds, NULL, NULL, &tv);
        if (res < 0 && errno != EINTR) {
            perror("select");
            pthread_exit((void *)-1);
        }

        if (res <= 0) {
            continue;
        }

        /*pthread_testcancel();*/
    }
    pthread_exit((void *) 0);
}

void eloop_terminate(void)
{
	eloop.terminate = 1;
}


void eloop_destroy(void)
{
	struct eloop_timeout *timeout, *prev;

	timeout = eloop.timeout;
	while (timeout != NULL) {
		prev = timeout;
		timeout = timeout->next;
		free(prev);
	}
	free(eloop.readers);
	free(eloop.signals);
}


int eloop_terminated(void)
{
	return eloop.terminate;
}


void * eloop_get_user_data(void)
{
	return eloop.user_data;
}




int thd_eloop_init(eloop_data_t *eloop_ptr, void *user_data)
{
    if (!eloop_ptr) {
		return -1;
    }

	memset(eloop_ptr, 0, sizeof(eloop_data_t));
	eloop_ptr->user_data = user_data;
    return 0;
}


int thd_eloop_register_read_sock(eloop_data_t *eloop_ptr, int sock,
			     void (*handler)(int sock, void *eloop_ctx,
					     void *sock_ctx),
			     void *eloop_data, void *user_data)
{
	struct eloop_sock *tmp;

    if (!eloop_ptr) {
		return -1;
    }

	tmp = (struct eloop_sock *)
		realloc(eloop_ptr->readers,
			(eloop_ptr->reader_count + 1) * sizeof(struct eloop_sock));
	if (tmp == NULL)
		return -1;

	tmp[eloop_ptr->reader_count].sock = sock;
	tmp[eloop_ptr->reader_count].eloop_data = eloop_data;
	tmp[eloop_ptr->reader_count].user_data = user_data;
	tmp[eloop_ptr->reader_count].handler = handler;
	eloop_ptr->reader_count++;
	eloop_ptr->readers = tmp;
	if (sock > eloop_ptr->max_sock)
		eloop_ptr->max_sock = sock;

	return 0;
}


int thd_eloop_unregister_read_sock(eloop_data_t *eloop_ptr, int sock)
{
	int i;

    if (!eloop_ptr) {
		return -1;
    }

	for (i = 0; i < eloop_ptr->reader_count; i++) {
		if (eloop_ptr->readers[i].sock == sock)
			break;
	}

	if (i >= eloop_ptr->reader_count)
		return -1;

	if (i + 1 < eloop_ptr->reader_count)
		memmove(&eloop_ptr->readers[i], &eloop_ptr->readers[i + 1],
			(eloop_ptr->reader_count - i - 1) *
			sizeof(struct eloop_sock));
	eloop_ptr->reader_count--;

	/* max_sock for select need not be exact, so no need to update it */
	/* don't bother reallocating block, since this area is quite small and
	 * next registration will realloc anyway */

	return 0;
}


int thd_eloop_register_timeout(eloop_data_t *eloop_ptr,
				unsigned int secs, unsigned int usecs,
			   void (*handler)(void *eloop_ctx, void *timeout_ctx),
			   void *eloop_data, void *user_data)
{
	struct eloop_timeout *timeout, *tmp, *prev;

    if (!eloop_ptr) {
		return -1;
    }

	timeout = (struct eloop_timeout *) malloc(sizeof(*timeout));
	if (timeout == NULL)
		return -1;
	eloop_gettime(&timeout->time, NULL);
	timeout->time.tv_sec += secs;
	timeout->time.tv_usec += usecs;
	while (timeout->time.tv_usec >= 1000000) {
		timeout->time.tv_sec++;
		timeout->time.tv_usec -= 1000000;
	}
	timeout->eloop_data = eloop_data;
	timeout->user_data = user_data;
	timeout->handler = handler;
	timeout->next = NULL;

	if (eloop_ptr->timeout == NULL) {
		eloop_ptr->timeout = timeout;
		return 0;
	}

	prev = NULL;
	tmp = eloop_ptr->timeout;
	while (tmp != NULL) {
		if (timercmp(&timeout->time, &tmp->time, <))
			break;
		prev = tmp;
		tmp = tmp->next;
	}

	if (prev == NULL) {
		timeout->next = eloop_ptr->timeout;
		eloop_ptr->timeout = timeout;
	} else {
		timeout->next = prev->next;
		prev->next = timeout;
	}

	return 0;
}


int thd_eloop_cancel_timeout(eloop_data_t *eloop_ptr,
			void (*handler)(void *eloop_ctx, void *sock_ctx),
			void *eloop_data, void *user_data)
{
	struct eloop_timeout *timeout, *prev, *next;
	int removed = 0;

    if (!eloop_ptr) {
		return -1;
    }

	prev = NULL;
	timeout = eloop_ptr->timeout;
	while (timeout != NULL) {
		next = timeout->next;

		if (timeout->handler == handler &&
		    (timeout->eloop_data == eloop_data ||
		     eloop_data == ELOOP_ALL_CTX) &&
		    (timeout->user_data == user_data ||
		     user_data == ELOOP_ALL_CTX)) {
			if (prev == NULL)
				eloop_ptr->timeout = next;
			else
				prev->next = next;
			free(timeout);
			removed++;
		} else
			prev = timeout;

		timeout = next;
	}

	return removed;
}


int thd_eloop_run(eloop_data_t *eloop_ptr)
{
	fd_set rfds;
	int i, res;
	struct timeval tv, now, tv_dflt;

    if (!eloop_ptr) {
		return -1;
    }

	while (!eloop_ptr->terminate &&
		(eloop_ptr->timeout || eloop_ptr->reader_count > 0)) {
		if (eloop_ptr->timeout) {
			eloop_gettime(&now, NULL);
#if 1
			/* Workaround for faulty kernels - cannot allow
			 * tv_usec to be >= 1000000 because otherwise tv_usec
			 * for select might be invalid and select() would fail
			 * with "Invalid argument". This error has been noticed
			 * at least with MIPS kernel and idt438. */
			if (now.tv_usec < 0 || now.tv_usec >= 1000000) {
				printf("ERROR: eloop_gettime returned invalid "
				       "time: tv_sec=%d tv_usec=%d\n",
				       (int) now.tv_sec, (int) now.tv_usec);
				now.tv_usec = 999999;
			}
#endif
			if (timercmp(&now, &eloop_ptr->timeout->time, <))
				timersub(&eloop_ptr->timeout->time, &now, &tv);
			else
				tv.tv_sec = tv.tv_usec = 0;
#if 0
			printf("next timeout in %lu.%06lu sec\n",
			       tv.tv_sec, tv.tv_usec);
#endif
		}

		FD_ZERO(&rfds);
		for (i = 0; i < eloop_ptr->reader_count; i++)
			FD_SET(eloop_ptr->readers[i].sock, &rfds);

		int max_fd = eloop_ptr->max_sock + 1;

        if (NULL != eloop_ptr->agent) {
			max_fd = (max_fd > eloop_ptr->max_sock ? (max_fd+1) : (eloop_ptr->max_sock+1));
		}
		/*
         * hlv, 2009-7-20
         * bug 7211, set block tv to 1sec, otherwise, it might
         * cause the delayed eloop terminate issue (terminated
         * flag would only be checked after the nearest timeout
         * event).
         *
         * bug 7358, if the nearest timeout event is less than
         * 'tv_dflt', set block time to tv.
         */
        tv_dflt.tv_sec = 1;
        tv_dflt.tv_usec = 0;
        if (timercmp(&tv_dflt, &tv, <)) {
            tv.tv_sec = tv_dflt.tv_sec;
            tv.tv_usec = tv_dflt.tv_usec;
        }

        res = select(max_fd, &rfds, NULL, NULL,
                eloop_ptr->timeout ? &tv : NULL);
		if (res < 0 && errno != EINTR) {
			perror("select");
			return -1;
		}

		/* check if some registered timeouts have occurred */
		if (eloop_ptr->timeout) {
			struct eloop_timeout *tmp;

			eloop_gettime(&now, NULL);
			if (!timercmp(&now, &eloop_ptr->timeout->time, <)) {
				tmp = eloop_ptr->timeout;
				eloop_ptr->timeout = eloop_ptr->timeout->next;
				tmp->handler(tmp->eloop_data,
					     tmp->user_data);
				free(tmp);
			}

		}

		if (res <= 0)
			continue;

		for (i = 0; i < eloop_ptr->reader_count; i++) {
			if (FD_ISSET(eloop_ptr->readers[i].sock, &rfds)) {
				eloop_ptr->readers[i].handler(
					eloop_ptr->readers[i].sock,
					eloop_ptr->readers[i].eloop_data,
					eloop_ptr->readers[i].user_data);
			}
		}
	}
	return 0;
}


int thd_eloop_terminate(eloop_data_t *eloop_ptr)
{
    if (!eloop_ptr) {
		return -1;
    }

	eloop_ptr->terminate = 1;
	return 0;
}


int thd_eloop_destroy(eloop_data_t *eloop_ptr)
{
	struct eloop_timeout *timeout, *prev;

    if (!eloop_ptr) {
		return -1;
    }

	timeout = eloop_ptr->timeout;
	while (timeout != NULL) {
		prev = timeout;
		timeout = timeout->next;
		free(prev);
	}
	free(eloop_ptr->readers);
	free(eloop_ptr->signals);
	return 0;
}


int thd_eloop_terminated(eloop_data_t *eloop_ptr)
{
    if (!eloop_ptr) {
		return -1;
    }

	return eloop_ptr->terminate;
}


void * thd_eloop_get_user_data(eloop_data_t *eloop_ptr)
{
	return eloop_ptr->user_data;
}

