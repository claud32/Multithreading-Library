#include <stdio.h>
#include <stdlib.h>

#include "uthread.h"

/* Print msg when thread1 got run*/
int thread1(void)
{
    printf("Thread1 got the cpu!\n");
    return 0;
}

/* Create thread 1 and take control of the CPU forever with 
 * an infinite while loop to test if preempt works */
int thread0(void)
{
    if (uthread_join(uthread_create(thread1), NULL) == -1) {
        perror("thread1 not created");
        exit(1);
    }
    
    while (1) {

    }
    return 0;
}

int main(void)
{
    if (uthread_start(1) == -1) {
        perror("uthread_start");
        exit(1);
    }
    if (uthread_join(uthread_create(thread0), NULL) == -1) {
        perror("uthread_join");
        exit(1);
    }
    return 0;
}