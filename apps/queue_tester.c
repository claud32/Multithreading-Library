#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

/* Delete first match item */
void test_delete_1(void)
{
	int matchYoung = 3, *ptr; 
	int matchOld = 3;
	int unmatch = 1;
	queue_t q;

	fprintf(stderr, "*** TEST delete 1***\n");

	q = queue_create();
	queue_enqueue(q, &unmatch);
	queue_enqueue(q, &matchOld);
	queue_enqueue(q, &matchYoung);
	queue_delete(q, &matchOld);
	queue_dequeue(q, (void**)&ptr);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &matchYoung);
}

/* Delete first item in queue */
void test_delete_2(void)
{
	int matchYoung = 3, *ptr; 
	int matchOld = 3;
	int unmatch = 1;
	queue_t q;

	fprintf(stderr, "*** TEST delete 2***\n");

	q = queue_create();
	queue_enqueue(q, &unmatch);
	queue_enqueue(q, &matchOld);
	queue_enqueue(q, &matchYoung);
	queue_delete(q, &unmatch);
	queue_delete(q, &matchOld);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &matchYoung);
}

/* Delete last item in queue */
void test_delete_3(void)
{
	int matchYoung = 3, *ptr; 
	int matchOld = 3;
	int unmatch = 1;
	queue_t q;

	fprintf(stderr, "*** TEST delete 3***\n");

	q = queue_create();
	queue_enqueue(q, &unmatch);
	queue_enqueue(q, &matchOld);
	queue_enqueue(q, &matchYoung);
	queue_delete(q, &matchYoung);
	queue_delete(q, &matchOld);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &unmatch);
}

/* Callback func: Increase int item by 1 or delete item 
 * if it has a value of 3. */
static int inc_item(queue_t q, void *data, void *arg)
{
    int *item_ptr = (int*)data;
    int inc = (int)(long)arg;

    if (*item_ptr == 3)
        queue_delete(q, data);
    else
        *item_ptr += inc;

    return 0;
}

/* Iterate */
void test_iterate(void)
{
	int testData[] = {4, 5, 3, 7};
	int *ptr;
	size_t i;
	queue_t q;

	fprintf(stderr, "*** TEST iterate ***\n");

	/* Initialize the queue and enqueue items */
	q = queue_create();
	for (i = 0; i < sizeof(testData) / sizeof(testData[0]); i++)
		queue_enqueue(q, &testData[i]);

    /* Add value '10' to every item of the queue, delete item '3' */
    queue_iterate(q, inc_item, (void*)10, NULL);
	queue_dequeue(q, (void**)&ptr);
    TEST_ASSERT(*ptr == 14);
    TEST_ASSERT(queue_length(q) == 2);
}

/*enqueue/dequeue 1*/
void test_en_dequeue_1(void)
{
	int data[4] = {0,1,2,3}, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST enqueue/dequeue 1***\n");

	q = queue_create();
	queue_enqueue(q, &data[0]);
	queue_enqueue(q, &data[1]);
	queue_enqueue(q, &data[1]);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data[0]);
}

/*enqueue/dequeue 2*/
void test_en_dequeue_2(void)
{
	int data[4] = {0,1,2,3}, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST enqueue/dequeue 2***\n");

	q = queue_create();
	queue_enqueue(q, &data[0]);
	queue_enqueue(q, &data[1]);
	queue_dequeue(q, (void**)&ptr);
	queue_enqueue(q, &data[0]);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data[1]);
}

/*enqueue/dequeue 3*/
void test_en_dequeue_3(void)
{
	int data[4] = {0,1,2,3}, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST enqueue/dequeue 3***\n");

	q = queue_create();
	queue_enqueue(q, &data[0]);
	queue_enqueue(q, &data[1]);
	queue_dequeue(q, (void**)&ptr);
	queue_enqueue(q, &data[0]);
	queue_dequeue(q, (void**)&ptr);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data[0]);
}

/*enqueue/dequeue 4*/
void test_en_dequeue_4(void)
{
	int data[4] = {0,1,2,3}, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST enqueue/dequeue 4***\n");

	q = queue_create();
	queue_enqueue(q, &data[0]);
	queue_enqueue(q, &data[1]);
	queue_dequeue(q, (void**)&ptr);
	queue_enqueue(q, &data[0]);
	queue_dequeue(q, (void**)&ptr);
	queue_dequeue(q, (void**)&ptr);
	queue_enqueue(q, &data[2]);
	queue_enqueue(q, &data[3]);
	queue_enqueue(q, ptr);
	queue_dequeue(q, (void**)&ptr);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data[3]);
}

/*error handle 1*/
void test_error_1(void)
{
	int data[4] = {0,1,2,3}, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST error handle 1***\n");

	q = queue_create();
	queue_enqueue(q, &data[0]);
	queue_dequeue(q, (void**)&ptr);

	TEST_ASSERT(queue_dequeue(q, (void**)&ptr) == -1);
}

/*error handle 2*/
void test_error_2(void)
{
	int data[4] = {0,1,2,3}, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST error handle 2***\n");

	q = queue_create();
	queue_enqueue(q, &data[0]);
	queue_dequeue(q, (void**)&ptr);

	TEST_ASSERT(queue_delete(q, &data[3]) == -1);
}


int main(void)
{
	test_create();
	test_queue_simple();
	test_delete_1();
	test_delete_2();
	test_delete_3();
	test_iterate();
	test_en_dequeue_1();
	test_en_dequeue_2();
	test_en_dequeue_3();
	test_en_dequeue_4();
	test_error_1();
	test_error_2();
	return 0;
}
