/*
 * Thread creation and yielding test
 *
 * Tests the creation of multiples threads and the fact that a parent thread
 * should get returned to before its child is executed. The way the printing,
 * thread creation and yielding is done, the program should output:
 *
 * thread4
 * thread3
 * thread2
 * thread1
 */

#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

int thread(void)
{
	uthread_yield();
	printf("thread%d\n", uthread_self());
	return 0;
}

int thread4(void)
{
	uthread_yield();
	printf("thread%d\n", uthread_self());
	return 0;
}

int thread3(void)
{
	uthread_join(uthread_create(thread4), NULL);
	uthread_yield();
	printf("thread%d\n", uthread_self());
	return 0;
}

int thread2(void)
{
	uthread_join(uthread_create(thread3), NULL);
	uthread_yield();
	printf("thread%d\n", uthread_self());
	return 0;
}

int thread1(void)
{
	uthread_join(uthread_create(thread2),NULL);
	// uthread_create(thread2);
	// uthread_yield();
	printf("thread%d\n", uthread_self());
	uthread_yield();
	return 0;
}

int main(void)
{
	uthread_start(1);
	uthread_join(uthread_create(thread1),NULL);
	// uthread_create(thread);
	uthread_stop();

	return 0;
}
