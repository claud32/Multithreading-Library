#ifndef _UTHREAD_H
#define _UTHREAD_H

/*
 * uthread_t - Thread identifier (TID) type
 *
 * Each user thread is assigned a different TID. TID are assigned in increasing
 * order and numbered starting from 1 (apart from the 'main' thread who
 * automatically gets TID #0). Overflowing the current TID value is considered a
 * case of failure (in other words, it is impossible to create more than
 * USHRT_MAX threads).
 */
typedef unsigned short uthread_t;

/*
 * uthread_func_t - Thread function type
 *
 * Return: Integer value
 */
typedef int (*uthread_func_t)(void);

/*
 * uthread_start - Start the multithreading library
 * @preempt: Preemption enable
 *
 * This function should only be called by the process' original execution
 * thread. It starts the multithreading scheduling library, and registers the
 * calling thread as the 'main' user-level thread (TID 0). If @preempt is
 * `true`, then preemptive scheduling is enabled.
 *
 * Return: 0 in case of success, -1 in case of failure (e.g., memory
 * allocation).
 */
int uthread_start(int preempt);

/*
 * uthread_stop - Stop the multithreading library
 *
 * This function should only be called by the main execution thread of the
 * process. It stops the multithreading scheduling library if there are no more
 * user threads.
 *
 * Return: 0 in case of success, -1 in case of failure.
 */
int uthread_stop(void);

/*
 * uthread_create - Create a new thread
 * @func: Function to be executed by the thread
 *
 * This function creates a new thread running the function @func and returns the
 * TID of this new thread.
 *
 * Return: -1 in case of failure (memory allocation, context creation, TID
 * overflow, etc.), or the TID of the new thread.
 */
int uthread_create(uthread_func_t func);

/*
 * uthread_self - Get thread identifier
 *
 * Return: The TID of the currently running thread
 */
uthread_t uthread_self(void);

/*
 * uthread_yield - Yield execution
 *
 * This function is to be called from the currently active and running thread in
 * order to yield for other threads to execute.
 */
void uthread_yield(void);

/*
 * uthread_exit - Exit from currently running thread
 * @retval: Return value
 *
 * This function is to be called from the currently active and running thread in
 * order to finish its execution. The return value @retval is to be collected
 * from a joining thread.
 *
 * A thread which has not been 'collected' should stay in a zombie state. This
 * means that until collection, the resources associated to a zombie thread
 * should not be freed.
 *
 * This function shall never return.
 */
void uthread_exit(int retval);

/*
 * uthread_join - Join a thread
 * @tid: TID of the thread to join
 * @retval: Address of an integer that will receive the return value
 *
 * This function makes the calling thread wait for the thread @tid to complete
 * and assign the return value of the finished thread to @retval (if @retval is
 * not NULL).
 *
 * A thread can be joined by only one other thread.
 *
 * Return: -1 if @tid is 0 (the 'main' thread cannot be joined), if @tid is the
 * TID of the calling thread, if thread @tid cannot be found, or if thread @tid
 * is already being joined. 0 otherwise.
 */
int uthread_join(uthread_t tid, int *retval);

#endif /* _THREAD_H */
