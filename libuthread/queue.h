#ifndef _QUEUE_H
#define _QUEUE_H

/*
 * queue_t - Queue type
 *
 * A queue is a FIFO data structure. Data items are enqueued one after the
 * other.  When dequeueing, the queue must returned the oldest enqueued item
 * first and so on.
 *
 * Apart from delete and iterate operations, all operations should be O(1).
 */
typedef struct queue* queue_t;

/*
 * queue_create - Allocate an empty queue
 *
 * Create a new object of type 'struct queue' and return its address.
 *
 * Return: Pointer to new empty queue. NULL in case of failure when allocating
 * the new queue.
 */
queue_t queue_create(void);

/*
 * queue_destroy - Deallocate a queue
 * @queue: Queue to deallocate
 *
 * Deallocate the memory associated to the queue object pointed by @queue.
 *
 * Return: -1 if @queue is NULL or if @queue is not empty. 0 if @queue was
 * successfully destroyed.
 */
int queue_destroy(queue_t queue);

/*
 * queue_enqueue - Enqueue data item
 * @queue: Queue in which to enqueue item
 * @data: Address of data item to enqueue
 *
 * Enqueue the address contained in @data in the queue @queue.
 *
 * Return: -1 if @queue or @data are NULL, or in case of memory allocation error
 * when enqueing. 0 if @data was successfully enqueued in @queue.
 */
int queue_enqueue(queue_t queue, void *data);

/*
 * queue_dequeue - Dequeue data item
 * @queue: Queue in which to dequeue item
 * @data: Address of data pointer where item is received
 *
 * Remove the oldest item of queue @queue and assign this item (the value of a
 * pointer) to @data.
 *
 * Return: -1 if @queue or @data are NULL, or if the queue is empty. 0 if @data
 * was set with the oldest item available in @queue.
 */
int queue_dequeue(queue_t queue, void **data);

/*
 * queue_delete - Delete data item
 * @queue: Queue in which to delete item
 * @data: Data to delete
 *
 * Find in queue @queue, the first (ie oldest) item equal to @data and delete
 * this item.
 *
 * Return: -1 if @queue or @data are NULL, of if @data was not found in the
 * queue. 0 if @data was found and deleted from @queue.
 */
int queue_delete(queue_t queue, void *data);

/*
 * queue_func_t - Queue callback function type
 * @queue: Queue to which item belongs
 * @data: Data item
 * @arg: Extra argument
 *
 * Return: 0 to continue iterating, 1 to stop iterating at this particular item.
 */
typedef int (*queue_func_t)(queue_t queue, void *data, void *arg);

/*
 * queue_iterate - Iterate through a queue
 * @queue: Queue to iterate through
 * @func: Function to call on each queue item
 * @arg: (Optional) Extra argument to be passed to the callback function
 * @data: (Optional) Address of data pointer where an item can be received
 *
 * This function iterates through the items in the queue @queue, from the oldest
 * item to the newest item, and calls the given callback function @func on each
 * item. The callback function receives the current data item and @arg.
 *
 * If @func returns 1 for a specific item, the iteration stops prematurely. In
 * this case only, if @data is different than NULL, then @data receives the data
 * item where the iteration was stopped.
 *
 * Note that this function should be resistant to data items being deleted
 * as part of the iteration (ie in @func).
 *
 * Return: -1 if @queue or @func are NULL, 0 otherwise.
 */
int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data);

/*
 * queue_length - Queue length
 * @queue: Queue to get the length of
 *
 * Return the length of queue @queue.
 *
 * Return: -1 if @queue is NULL. Length of @queue otherwise.
 */
int queue_length(queue_t queue);

#endif /* _QUEUE_H */
