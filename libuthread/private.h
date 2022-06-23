#ifndef _UTHREAD_PRIVATE_H
#define _UTHREAD_PRIVATE_H

/*
 * This header is only meant to be included by files from the libuthread, as it
 * defines some private APIs usable internally. This header is not to be
 * included by user programs directly.
 */

/**
 * Private context API
 */
#include <ucontext.h>

#include "uthread.h"

/*
 * uthread_ctx_t - User-level thread context
 *
 * This type is an opaque data structure type that contains a thread's execution
 * context.
 *
 * Such a context is initialized for the first time when creating a thread with
 * uthread_ctx_init(). Once initialized, it can be switched to with
 * uthread_ctx_switch().
 */
typedef ucontext_t uthread_ctx_t;

/*
 * uthread_ctx_switch - Switch between two execution contexts
 * @prev: Pointer to the execution context structure in which to save the
 *	currently running thread
 * @next: Pointer to the execution context structure to resume
 */
void uthread_ctx_switch(uthread_ctx_t *prev, uthread_ctx_t *next);

/*
 * uthread_ctx_alloc_stack - Allocate stack segment
 *
 * Return: Pointer to the top of a valid stack segment, or NULL in case of
 * failure
 */
void *uthread_ctx_alloc_stack(void);

/*
 * uthread_ctx_destroy_stack - Deallocate stack segment
 * @top_of_stack: Address of stack to deallocate
 */
void uthread_ctx_destroy_stack(void *top_of_stack);

/*
 * uthread_ctx_init - Initialize a thread's execution context
 * @uctx: Pointer to thread context to initialize
 * @top_of_stack: Pointer to the top of a valid stack segment, as allocated by
 *	uthread_ctx_alloc_stack()
 * @func: Function to be executed by the thread
 *
 * Return: 0 if @uctx was properly initialized, or -1 in case of failure
 */
int uthread_ctx_init(uthread_ctx_t *uctx, void *top_of_stack,
					 uthread_func_t func);


/**
 * Private preemption API
 */

/*
 * preempt_start - Start thread preemption
 *
 * Configure a timer that must fire a virtual alarm at a frequency of 100 Hz and
 * setup a timer handler that forcefully yields the currently running thread.
 */
void preempt_start(void);

/*
 * preempt_stop - Stop thread preemption
 *
 * Restore previous timer configuration, and previous action associated to
 * virtual alarm signals.
 */
void preempt_stop(void);

/*
 * preempt_enable - Enable preemption
 */
void preempt_enable(void);

/*
 * preempt_disable - Disable preemption
 */
void preempt_disable(void);

#endif /* _UTHREAD_PRIVATE_H */
