/*
 * Event loop
 * Copyright 2002-2003, Jouni Malinen <jkmaline@cc.hut.fi>
 * All Rights Reserved.
 */

#ifndef ELOOP_H
#define ELOOP_H

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>


/* Magic number for eloop_cancel_timeout() */
#define ELOOP_ALL_CTX (void *) -1

struct eloop_sock {
	int sock;
	void *eloop_data;
	void *user_data;
	void (*handler)(int sock, void *eloop_ctx, void *sock_ctx);
};

struct eloop_timeout {
	struct timeval time;
	void *eloop_data;
	void *user_data;
	void (*handler)(void *eloop_ctx, void *sock_ctx);
	struct eloop_timeout *next;
};

struct eloop_signal {
	int sig;
	void *user_data;
	void (*handler)(int sig, void *eloop_ctx, void *signal_ctx);
	int signaled;
};

typedef struct eloop_data {
	void *user_data;

	int max_sock, reader_count;
	struct eloop_sock *readers;

	struct eloop_timeout *timeout;

	int signal_count;
	struct eloop_signal *signals;
	int signaled;

	int terminate;

	void *agent;
}eloop_data_t;

/* Initialize global event loop data - must be called before any other eloop_*
 * function. user_data is a pointer to global data structure and will be passed
 * as eloop_ctx to signal handlers. */
void eloop_init(void *user_data);
int thd_eloop_init(eloop_data_t *eloop_ptr, void *user_data);

/* Register handler for read event */
int eloop_register_read_sock(int sock,
			     void (*handler)(int sock, void *eloop_ctx,
					     void *sock_ctx),
			     void *eloop_data, void *user_data);
int thd_eloop_register_read_sock(eloop_data_t *eloop_ptr, int sock,
			     void (*handler)(int sock, void *eloop_ctx,
					     void *sock_ctx),
			     void *eloop_data, void *user_data);
int eloop_unregister_read_sock(int sock);
int thd_eloop_unregister_read_sock(eloop_data_t *eloop_ptr, int sock);

/* Register timeout */
int eloop_register_timeout(unsigned int secs, unsigned int usecs,
			   void (*handler)(void *eloop_ctx, void *timeout_ctx),
			   void *eloop_data, void *user_data);
int thd_eloop_register_timeout(eloop_data_t *eloop_ptr,
				unsigned int secs, unsigned int usecs,
			   void (*handler)(void *eloop_ctx, void *timeout_ctx),
			   void *eloop_data, void *user_data);

/* Cancel timeouts matching <handler,eloop_data,user_data>.
 * ELOOP_ALL_CTX can be used as a wildcard for cancelling all timeouts
 * regardless of eloop_data/user_data. */
int eloop_cancel_timeout(void (*handler)(void *eloop_ctx, void *sock_ctx),
			 void *eloop_data, void *user_data);
int thd_eloop_cancel_timeout(eloop_data_t *eloop_ptr,
			void (*handler)(void *eloop_ctx, void *sock_ctx),
			void *eloop_data, void *user_data);

/* Register handler for signal.
 * Note: signals are 'global' events and there is no local eloop_data pointer
 * like with other handlers. The (global) pointer given to eloop_init() will be
 * used as eloop_ctx for signal handlers. */
int eloop_register_signal(int sock,
			  void (*handler)(int sig, void *eloop_ctx,
					  void *signal_ctx),
			  void *user_data);

/* Start event loop and continue running as long as there are any registered
 * event handlers. */
void eloop_run(void);
void *eloop_run_thread(void *arg);
int thd_eloop_run(eloop_data_t *eloop_ptr);

/* Terminate event loop even if there are registered events. */
void eloop_terminate(void);
int thd_eloop_terminate(eloop_data_t *eloop_ptr);

/* Free any reserved resources. After calling eloop_destoy(), other eloop_*
 * functions must not be called before re-running eloop_init(). */
void eloop_destroy(void);
int thd_eloop_destroy(eloop_data_t *eloop_ptr);

/* Check whether event loop has been terminated. */
int eloop_terminated(void);
int thd_eloop_terminated(eloop_data_t *eloop_ptr);

/* Return user_data pointer that was registered with eloop_init() */
void * eloop_get_user_data(void);
void * thd_eloop_get_user_data(eloop_data_t *eloop_ptr);

/* Connect to cmm agent */
void eloop_connect_cmm_agent(void *agent);


#endif /* ELOOP_H */
