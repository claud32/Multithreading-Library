#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* The queue is implemented by a double linked list
 * a node can visit its last and next node */
struct Node {
	void *data;
	struct Node *next_node;
	struct Node *last_node;
};

struct queue {
	struct Node *front;
	struct Node *rear;
	int length;
};

queue_t queue_create(void)
{
	struct queue *Q = malloc(sizeof(struct queue));

	/* malloc failed */
	if (Q == NULL)
		return NULL;

	/* initialize queue */
	Q->front = NULL;
	Q->rear = NULL;
	Q->length = 0;

	return Q;

}

int queue_destroy(queue_t queue)
{
	if (queue == NULL || queue->length > 0) 
		return -1;

	free(queue);

	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{

	if (queue == NULL || data == NULL)
		return -1;

	struct Node *N = malloc(sizeof(struct Node)); 

	/* malloc failed */
	if (N == NULL)
		return -1;

	N->data = data;
	N->next_node = NULL;
	N->last_node = NULL;

	if (queue->length == 0) {
		/* for empty queue, front and rear should point to the same node */
		queue->front = N;
		queue->rear = N;
	} else {
		/* add the new node at the end of the queue */
		N->last_node = queue->rear;
		queue->rear->next_node = N; 
		queue->rear = N;
	}

	queue->length = queue->length + 1;

	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	
	if (queue->length == 0 || queue == NULL || data == NULL)
		return -1;

	/* store the first node */
	struct Node *first_node = queue->front;

	/* dequeue the first node*/
	*data = queue->front->data;
	first_node = queue->front;
	queue->front = queue->front->next_node;

	/* recycle the memory address */
	free(first_node);

	/* if the queue is empty after dequeue */
	if (queue->front == NULL)
		queue->rear = NULL;
	
	queue->length = queue->length - 1;

	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	if (queue == NULL || queue->length == 0 || data == NULL)
		return -1; 

	/* set default status to -1, failure */
	int delete_status = -1;
	struct Node *current_node = queue->front;
	for (int i = 1; i < queue->length; i++) {
		if (current_node->data == data) {

			if (current_node->last_node == NULL) {
				queue->front = current_node->next_node;
				current_node->next_node->last_node = NULL;
			} else {
				/* link the last_node to the next_node */
				current_node->last_node->next_node = current_node->next_node;
			}

			queue->length = queue->length - 1;
			if (queue->length == 0) {
				queue->front = NULL;
				queue->rear = NULL;
			}

			/* recycle the memory address */
			free(current_node);
			/* change the status to 0, success */
			delete_status = 0;
			break;
		} else {
			current_node = current_node->next_node;
		}
	}
	return delete_status;
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
	if (queue == NULL || func == NULL)
		return -1;

	struct Node *current_node = queue->front;
	void *current_data;

	for (int i = 0; i < queue->length; i++) {
		current_data = current_node->data;
		/* iterate the queue */
		current_node = current_node->next_node;
		/* apply the function */ 
		if ((*func)(queue, current_data, arg)) {
			if (data != NULL)
				*data = current_data;
			break;
		}
	}

	return 0;

}

int queue_length(queue_t queue)
{
	if (queue == NULL)
		return -1; 
	return queue->length;
}


