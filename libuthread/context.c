#include <stdio.h>
#include <stdlib.h>

#include "private.h"
#include "uthread.h"

/* Size of the stack for a thread (in bytes) */
#define UTHREAD_STACK_SIZE 32768

void uthread_ctx_switch(uthread_ctx_t *prev, uthread_ctx_t *next)
{
	/*
	 * swapcontext() saves the current context in structure pointer by @prev
	 * and actives the context pointed by @next
	 */
	if (swapcontext(prev, next)) {
		perror("swapcontext");
		exit(1);
	}
}

void *uthread_ctx_alloc_stack(void)
{
	return malloc(UTHREAD_STACK_SIZE);
}

void uthread_ctx_destroy_stack(void *top_of_stack)
{
	free(top_of_stack);
}

/*
 * uthread_ctx_bootstrap - Thread context bootstrap function
 * @func: Function to be executed by the new thread
 */
static void uthread_ctx_bootstrap(uthread_func_t func)
{
	/*
	 * Enable interrupts right after being elected to run for the first time
	 */
	preempt_enable();

	/* Execute thread and when done, exit with the return value */
	uthread_exit(func());
}

int uthread_ctx_init(uthread_ctx_t *uctx, void *top_of_stack,
		     uthread_func_t func)
{
	/*
	 * Initialize the passed context @uctx to the currently active context
	 */
	if (getcontext(uctx))
		return -1;

	/*
	 * Change context @uctx's stack to the specified stack
	 */
	uctx->uc_stack.ss_sp = top_of_stack;
	uctx->uc_stack.ss_size = UTHREAD_STACK_SIZE;

	/*
	 * Finish setting up context @uctx:
	 * - the context will jump to function uthread_ctx_bootstrap() when
	 *   scheduled for the first time
	 * - when called, function uthread_ctx_bootstrap() will receive @func
	 */
	makecontext(uctx, (void (*)(void)) uthread_ctx_bootstrap, 1, func);

	return 0;
}

