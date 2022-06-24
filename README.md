# Multithreading-Library
A multithreading library for Linux

Video demonstration available at: https://claud.pro/multithreading/

## High-level Design Summary
* The overall architecture of the User-level thread library consist of three main modules (```Queue API```, ```uthread API```, and ```Preemption```) which are technically separatable from each other but also interoperable. ```Queue API``` is a basal data structure. ```uthread API``` is a library for users to start multi-threaded function in a process. ```Preemption``` is an activatable feacture for the library for extra control. 
* The data structure choice: ```Queue``` which is FIFO. It is good for round-robin thread scheduling logic.
* Platform: For ```Linux``` in user-level.
* Relationship among modules: ```uthread API``` is build on top of ```Queue API``` while ```Preemption``` is a necessary added feature on top of ```uthread API```.
### Queue API
The Queue is implement by ```Doubly Linked List``` with ```FIFO``` rule. I choose this structure because it provides an efficient way to add or remove nodes. ```Doubly Linked List``` also allow us delete a node by simple linking the node before it and the node after it. ```FIFO``` is excatly what I need for thread scheduling.
### Queue Testing
* There are 12 unit tests.
* ```test_create``` and ```test_queue_simple``` are pre-given
* ```test_delete_1``` enqueues two match items and an unmatch item in the queue to test if ```queue_dequeue``` deletes the first match item near the head. In similar fashion, ```test_delete_2``` tests deletion of the head item and ```test_delete_3``` tests deletion of the tail item.
* ```test_iterate```enqueues 4 integers and applys a ```inc_item``` fucntion that increases int item by 1 or delete item if it has a value of 3 to each of the item. Assert head value and queue length to test if ```test_iterate``` has the right behaviour.
* ```test_en_dequeue_1```, ```test_en_dequeue_2```, ```test_en_dequeue_3```, ```test_en_dequeue_4```: more enqueue/dequeue actions to cove edge cases.
* ```test_error_1``` tests error handling when trying to ```queue_dequeue``` an empty queue. ```test_error_2``` tests error handling when trying to ```queue_delete``` an empty queue.

### uthread API
I designed a sturcture ```TCB``` to store info for a thread, including ```context```,```TID```,```state```, a stack for storing context and return value.
I have following global varibles
* ready_queue: stores active threads
* zombie_queue: stores zombie threads
* thread_count: count how many thread I have now and provide unique TID
* current_thread: a pointer to TCB which is currently running
* uthread_start  
In this function, I initialize global variables for the API, including ```ready_queue```, ```zombie_queue``` and ```thread_count```. Then I create a main thread(TID=0) and set it as current_thread. If ```preempt``` is 1, I will also call ```preempt_start``` to start using preempt.
* uthread_stop  
This is the final function I should call to stop running uthread API. I check if there is anything left in ```ready_queue```. If so, I ```uthread_join``` these threads and let them finish. Then I free everything I allocated, including global variables and anything left in the queues to prevent memory leak.
* uthread_create  
This function create a new thread. I allocate a TCB and a stack for it and put it at the end of our ```ready_queue```. Then I call ```uthread_ctx_init``` to initialize it. I want the whole process can be done safely, so I temporarily disable preempt at the beginning and enable it after the new thread was put in queue.
* uthread_exit  
This function deal with a finished thread. I stores return value in its TCB and put this finished thread into the ```zombie_queue```. Then I call ```uthread_ctx_switch``` to run next avaliable thread. Also, I disable preempt here.
* uthread_yield  
This function allows a thread yield and let next thread run. I put the current_thread to the end of ```ready_queue``` and dequeue a new thread from it. Then I call ```uthread_ctx_switch``` to run the new thread. Here, I also disable preempt to protect the whole process.
* uthread_join  
This function needs the parent thread to wait its child. So I use a infinite loop to make the parent "wait". In the loop, I check if the child thread is finished. I call ```queue_iterate``` to check if ```zombie_queue``` has the child thread. If not, parent thread will keep yield until the child finish. I check if the child thread is in the ```ready_queue``` in the beginning to prevent join something doesn't exist and waiting forever. 

### uthread API Testing
I basically implement 2 types of testing.   
* Let a thread create a lot of child threads  
* Let the child thread keep create child threads  
Then I mix the 2 type to implement stressful test on our API.  
Also, I use valgrind to check memory leak.  

### Preemption Feature
* ```sig_handler``` signal handler to ask a thread to yield by calling ```uthread_yield```
* ```preempt_start``` first mounts ```sig_handler``` to ```SIGVTALRM``` using ```sigaction``` then sets up a virtual alarm that sends ```SIGVTALRM``` by specifying certain ```tv_usec``` using ```setitimer```. This virtual alarm counts how long has the thread been using the CPU.
* ```preempt_stop``` first stops the timer using ```setitimer``` then restore ```SIG_DFL``` for ```SIGVTALRM```. This way it prevents accidental termination of thread.
* ```preempt_enable``` first creates a signal set ```unblock_alarm``` and adds it to the signal mask using ```sigaddset``` pairing with ```SIGVTALRM```, make the change for ```SIG_UNBLOCK``` signals with  ```sigprocmask```
* In similar fashion, ```preempt_disable``` first creates a signal set ```block_alarm``` and adds it to the signal mask using ```sigaddset``` pairing with ```SIGVTALRM```, make the change for ```SIG_BLOCK``` signals with  ```sigprocmask```

### Preemption Feature Testing
* The idea is simple, it schedules two thread ```thread0``` and ```thread1``` with Preemption on. ```thread0``` is going to take control of the CPU with a while-loop forever if Preemption doesn't work. ```thread1``` prints a message if it got the CPU.

## Reference
[pthread_join](https://man7.org/linux/man-pages/man3/pthread_join.3.html/)

[Memory leak check tool](https://valgrind.org/docs/manual/quick-start.html#quick-start.prepare)

[Signal-Actions](https://www.gnu.org/software/libc/manual/html_mono/libc.html#Signal-Actions)

[Basic-Signal-Handling](https://www.gnu.org/software/libc/manual/html_mono/libc.html#Basic-Signal-Handling)

[Signal-Function](https://www.gnu.org/software/libc/manual/html_mono/libc.html#Sigaction-Function-Example)

[Setting-an-Alarm](https://www.gnu.org/software/libc/manual/html_mono/libc.html#Setting-an-Alarm)

[Time-Types](https://www.gnu.org/software/libc/manual/html_mono/libc.html#Time-Types)

[Makefile](https://www.gnu.org/software/make/manual/make.html#Introduction)

[static-libraries](https://tldp.org/HOWTO/Program-Library-HOWTO/static-libraries.html)

[pthread_exit](https://man7.org/linux/man-pages/man3/pthread_exit.3.html)

[pthread_create](https://man7.org/linux/man-pages/man3/pthread_create.3.html)


