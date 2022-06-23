/*
 * Simple hello world test
 *
 * Tests the creation of a single thread and its successful return.
 */

#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

int hello(void)
{
	printf("Hello world!\n");
	return 0;
}

int main(void)
{
	uthread_t tid;
	uthread_start(0);

	tid = uthread_create(hello);

	uthread_join(tid, NULL);

	uthread_stop();
	return 0;
}
