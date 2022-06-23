#include <assert.h>
#include <limits.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "preempt.c"
#include "queue.h"
#include "context.c"

#define Running 0
#define Ready 1
#define Blocked 2
#define Zombie 3

struct TCB{
	uthread_t TID;
	uthread_ctx_t context;
	int state;
	void* stack;
	int retval;
};

/* stores current running TCB */
struct TCB* current_thread;

/* stores active and zombie threads TCB*/
static queue_t ready_queue, zombie_queue;

/* count the number of threads so I can provide unique TID*/
static int thread_count = 0;


int uthread_start(int preempt)
{
	if (preempt == 1)
		preempt_start();

	/* create queue for threads*/
	ready_queue = queue_create();
	zombie_queue = queue_create();

	/* create main thread */
	struct TCB *uthread_tcb = malloc(sizeof(struct TCB));

	/* malloc faliure */
	if (ready_queue == NULL || zombie_queue == NULL || uthread_tcb == NULL)
		return -1;

	/* main thread TID is 0 */
	uthread_tcb->TID = thread_count;
	uthread_tcb->state = Ready;

	/* set current thread as main thread */
	current_thread = uthread_tcb;

	return 0;
}

int uthread_stop(void)
{
	/* only main thread can call uthread */
	if (current_thread->TID != 0)
		return -1;
	
	struct TCB *tcb;

	/* if there is still active threads in ready queue
	 * the main thread will join them */
	while (queue_length(ready_queue) > 0) {
		queue_dequeue(ready_queue, (void**)&tcb);

		/* we don't need to join main thread*/
		if (tcb->TID == 0) {
			queue_enqueue(ready_queue, tcb);
			continue;
		}

		/* put the thread back so yield could let it run */
		queue_enqueue(ready_queue, tcb);
		uthread_join(tcb->TID, NULL);
	}

	/* free ready and zombie queues and anything in the queue */
	while (queue_length(ready_queue) > 0) {
		queue_dequeue(ready_queue, (void**)&tcb);
		free(tcb);
		free(tcb->stack);
	}
	while (queue_length(zombie_queue) > 0) {
		queue_dequeue(zombie_queue, (void**)&tcb);
		free(tcb);
		free(tcb->stack);
	}
	queue_destroy(ready_queue);
	queue_destroy(zombie_queue);

	/*free the main thread TCB */
	free(current_thread);

	preempt_stop();
	return 0;
}

int uthread_create(uthread_func_t func)
{
	/* malloc a new TCB for new thread */
	struct TCB *uthread_tcb = malloc(sizeof(struct TCB));

	/* malloc and change type to char* */
    uthread_tcb->stack = (char*)uthread_ctx_alloc_stack();

    /* protect the thread when creating new TCB */
    preempt_disable();

    /* initialize the tcb */
    int initial_status = uthread_ctx_init(&(uthread_tcb->context), uthread_tcb->stack, func);
    /* initialize faliure */
    if (initial_status == -1)
		return -1;
    /* set TID and state */
    thread_count++;
    uthread_tcb->TID = thread_count;
    uthread_tcb->state = Ready;

    /* put the thread into ready queue*/
    queue_enqueue(ready_queue, uthread_tcb);

    preempt_enable();

    return uthread_tcb->TID;
}

void uthread_yield(void)
{
	/* protect the thread when switch to new thread*/
	preempt_disable();

	/* yield thread will yield */
	struct TCB *yield_thread = current_thread;

	/* yield thread will go to the end of ready queue */
	queue_enqueue(ready_queue, yield_thread);

	/* next avaliable thread becomes current thread */
	queue_dequeue(ready_queue, (void**)&current_thread);

	/*switch context*/
	uthread_ctx_switch(&(yield_thread->context), &(current_thread->context));
}

uthread_t uthread_self(void)
{
	return current_thread->TID;
}

void uthread_exit(int retval)
{
	/* protect the thread when a thread is ready to finish */
	preempt_disable();

	struct TCB *zombie_thread = current_thread;
	/* get next available thread from queue as new current thread */
	queue_dequeue(ready_queue, (void**)&current_thread);
	/* change the status to zombie*/
	zombie_thread->state = Zombie;
	/* stores the return value in the TCB so parent could access to it */ 
	zombie_thread->retval = retval;
	/* put zombie_thread into zombie_queue so parent knows it finished */
	queue_enqueue(zombie_queue, zombie_thread);
	uthread_ctx_switch(&(zombie_thread->context), &(current_thread->context));

}


static int find_by_tid(queue_t q, void *data, void *arg)
{
	/* this instruction will basically do nothing
	 * q has no use here and it just avoids the usused variable warning
	 */
	if (queue_length(q) == 0)
		return 0;

	int tid = *((int*)arg);

	/* if find the TID, return true */
	return ((struct TCB*)data)->TID == tid;
}


int uthread_join(uthread_t tid, int *retval)
{
	/* main thread and a thread itself cannot be joined */
	if (tid == 0 || tid == uthread_self())
		return -1;

	struct TCB *tcb = NULL;

	/* make sure the TID we will join is in the ready queue*/
	queue_iterate(ready_queue, find_by_tid, (void*)&tid, (void**)&tcb);

	if (tcb == NULL)
		return -1;

	/* keep waiting until the child thread finish and is found in zombie queue*/
	while (1) {

		/* yield to next thread */
		uthread_yield();
		
		struct TCB *tcb = NULL;
		/* find if zombie_queue has the child thread */
		queue_iterate(zombie_queue, find_by_tid, (void*)&tid, (void**)&tcb);
		/* if found in zombie queue*/
		if (tcb != NULL) {
			/* get the return value */
			if (retval != NULL)
				*retval = tcb->retval;
			/* parent thread now can continue to run */
			break;
		}
	}

	return 0;
}

