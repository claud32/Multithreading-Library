#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100
/*
 * Convert HZ to microseconds
 * Good for usage in long int tv_usec for struct timeval in Time Types
 */
#define HZ_MICROSEC 1000000 / HZ 

/* 
 * sig_handler - signal handler to ask a thread to yield
 * 
 * This function shall never return. */
void sig_handler()
{
	uthread_yield();
}

void preempt_start(void)
{
	struct sigaction sa;
	struct itimerval new;

	/* Set up the structure to specify the new action */
	sa.sa_handler = sig_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaction(SIGVTALRM, &sa, NULL) != 0) {
		perror("sigaction in preempt_start");
		exit(1);
	}

	/* Set up the vitural timer for 100 HZ */
	new.it_interval.tv_usec = (long int) HZ_MICROSEC;
	new.it_interval.tv_sec = 0;
	new.it_value.tv_usec = (long int) HZ_MICROSEC;
	new.it_value.tv_sec = 0;
	if (setitimer(ITIMER_VIRTUAL, &new, NULL) != 0) {
		perror("setitimer in preempt_start");
		exit(1);
	}
}

void preempt_stop(void)
{
	struct sigaction sa;

	/* Stop the timer first */
	if (setitimer(ITIMER_VIRTUAL, NULL, NULL) != 0) {
		perror("setitimer in preempt_stop");
		exit(1);
	}

	/* Restore default signal action */
	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGVTALRM, &sa, NULL) != 0) {
		perror("sigaction in preempt_stop");
		exit(1);
	}
}

void preempt_enable(void)
{
	sigset_t unblock_alarm;

	/* Initialize the signal mask. */
	sigemptyset(&unblock_alarm);
	if (sigaddset(&unblock_alarm, SIGVTALRM) != 0) {
		perror("sigaddset in preempt_enable");
		exit(1);
	}

	if (sigprocmask(SIG_UNBLOCK, &unblock_alarm, NULL) != 0) {
		perror("sigprocmask in preempt_enable");
		exit(1);
	}
}

void preempt_disable(void)
{
	sigset_t block_alarm;

	/* Initialize the signal mask. */
	sigemptyset(&block_alarm);
	if (sigaddset(&block_alarm, SIGVTALRM) != 0) {
		perror("sigaddset in preempt_disable");
		exit(1);
	}
	
	if (sigprocmask(SIG_BLOCK, &block_alarm, NULL) != 0) {
		perror("sigprocmask in preempt_disable");
		exit(1);
	}
}

