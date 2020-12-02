#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <ucontext.h>
#include "queue.h"
#include <stdlib.h> 
#include <signal.h>
#include "sut.h"
#include "a1_lib.h"
#include <stdint.h>
#include <sys/socket.h>

typedef void (*sut_task_f)();

int numthreads, curthread;
threaddesc threadarr[100];
threaddesc *iodescarray[100];
threaddesc *runningfunction;
int activeThreads;
struct queue c_q;
void *c_exec();
void *i_exec();
ucontext_t m;
static pthread_mutex_t mmutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t rmutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t bmutex = PTHREAD_MUTEX_INITIALIZER;
ucontext_t m;
struct queue request_q;
struct queue c_q;
pthread_t CEXEC;
pthread_t IEXEC;
struct queue_entry* ptr;

struct queue_entry* ptr3; //for debugging
int iocounter;
int cexecisclosing;


void sut_init(){ 

	activeThreads = 0;
	iocounter =0;
	numthreads = 0;
	curthread = 0;

	c_q = queue_create();
	request_q = queue_create();

	queue_init(&c_q);
	queue_init(&request_q); 

	pthread_create(&CEXEC, NULL, c_exec, NULL);
	pthread_create(&IEXEC, NULL, i_exec, NULL);

}

bool sut_create(sut_task_f fn){
	

	pthread_mutex_lock(&mmutex);
	threaddesc *tdescptr;

	if (activeThreads >= 15) 
	{
		printf("FATAL: Maximum thread limit reached... creation failed! \n");
		return false;
	}
	
	tdescptr = &(threadarr[numthreads]);

	getcontext(&(tdescptr->threadcontext));

	tdescptr->threadid = curthread;
	tdescptr->threadstack = (char *)malloc(THREAD_STACK_SIZE);
	tdescptr->threadcontext.uc_stack.ss_sp = tdescptr->threadstack;
	tdescptr->threadcontext.uc_stack.ss_size = THREAD_STACK_SIZE;
	tdescptr->threadcontext.uc_link = 0;
	tdescptr->threadcontext.uc_stack.ss_flags = 0;
	tdescptr->threadfunc = *fn;
	tdescptr->sockfd = 99;

	makecontext(&(tdescptr->threadcontext), *fn, 0);
	
	struct queue_entry *node = queue_new_node(tdescptr);
    queue_insert_tail(&c_q, node);
    numthreads++;
    activeThreads++;
    curthread++;
    pthread_mutex_unlock(&mmutex);
   

    return true;
}

void *c_exec() { //procedure function for c exec thread
	

	while(1){ //awaiting input
		usleep(100);

		if(queue_peek_front(&c_q) != NULL){
			break;
		}
		else if(activeThreads == 0){
			goto EXITLABEL;
		}
	}

	while(1){ //we have input, deal with it
		usleep(100);

		pthread_mutex_lock(&mmutex);
    	if(queue_peek_front(&c_q) != NULL) {

    		ptr = queue_pop_head(&c_q);
    		pthread_mutex_unlock(&mmutex);
    		runningfunction = (threaddesc *)ptr->data;
    		
    		swapcontext(&m, &(runningfunction->threadcontext));	
    	}
    	else{
    		pthread_mutex_unlock(&mmutex);
    		break;
    	
    	}
    }

EXITLABEL: ;

	cexecisclosing=5;

	pthread_exit(NULL);
}

void *i_exec(){

	int tmpsockfd;

	while(1){ //waiting input
		usleep(100);
		//printf("#DEBUG# working threads: %d %d\n", activeThreads, cexecisclosing);
		if(queue_peek_front(&request_q) != NULL ){
			break;
		} 
		else if(cexecisclosing==5){
			goto EXITLABEL2;
		}
	}
	int flag;
	while(1){ //we have input deal with it

		//printf("#DEBUG# bruh1 is c exec closing: %d; flag:%d\n", cexecisclosing,flag);

		flag=0;
		usleep(100);

		pthread_mutex_lock(&rmutex);

    		//printf("#DEBUG# bruh2 is c exec closing: %d; flag: %d\n", cexecisclosing,flag);
			
    		struct queue_entry* ptr2 = queue_pop_head(&request_q); //ptr2 is a queue entry
    		
    		threaddesc *iothread= (threaddesc *)ptr2->data; //iothread is a thread descriptor
    		//printf("#DEBUG# recvd tasktype signal %d\n", iothread->tasktype); 

    		if(iothread->tasktype == 1){
    		//	printf("#DEBUG# wow we made it into i_exec->sutopen\n");
    			
    		//////////////
    		//SUT_OPEN://
    		//////////////
    			if (connect_to_server((const char *)iothread->dest, (uint16_t)iothread->port, &tmpsockfd) == -1) {
				printf("Connection to server failed\n");
				 
					flag=2;
					//pthread_mutex_unlock(&bmutex);
					goto READYLABEL;;//this makes us jump straight to requeuing the task back to task ready, while not performing the operation
				}
				printf("#DEBUG# server connection successful on port: %d\non dest: %s\n", iothread->port, iothread->dest);
				iothread->sockfd=tmpsockfd;
				//printf("#DEBUG# sockfd: %d\n",tmpsockfd);
				iothread->socketopen=1;
    		}

    		//////////////
    		//SUT_READ:///
    		//////////////

    		if(iothread->tasktype == 2) { 
    			if (iothread->sockfd==99 || iothread->socketopen ==0){

    				printf("Error! You haven't called sut_open yet!\n");
    				//pthread_mutex_unlock(&bmutex);
    				goto READYLABEL;; //this makes us jump straight to requeuing the task back to task ready, while not performing the operation
    			} //to be copy pasted potentially into write and close sections of iexec
    			//here we do read
    			recv_message(iothread->sockfd, (char *)iothread->recvd_msg, 1024);
    			iothread->recvd_signal =1;
    		}
    		//////////////
    		//SUT_WRITE://
    		//////////////

    		if(iothread->tasktype == 3) { //write
    			if (iothread->sockfd== 99 || iothread->socketopen ==0){

    				printf("Error! You haven't called sut_open yet!\n");
    				//pthread_mutex_unlock(&bmutex);
    				goto READYLABEL;; //this makes us jump straight to requeuing the task back to task ready, while not performing the operation
    			}
    			send_message(iothread->sockfd, (char*)iothread->write_msg, iothread->size_msg);
    			//here we do write
    		}

    		//////////////
    		//SUT_CLOSE://
    		//////////////

    		if(iothread->tasktype == 4) {
    			if (iothread->sockfd==99 || iothread->socketopen ==0){

    				printf("Error! You haven't called sut_open yet!\n");
    				
    				goto READYLABEL;; //this makes us jump straight to requeuing the task back to task ready, while not performing the operation
    			}
    			//shutdown(iothread->sockfd, 2);
    			close(iothread->sockfd);
    			iothread->socketopen = 0;
    		}
    		//we are done I/O! proceed to C-EXEC for the rest of the function
READYLABEL:;
			pthread_mutex_lock(&mmutex);

			struct queue_entry *node = queue_new_node(iothread);
   			queue_insert_tail(&c_q, node);
   			pthread_mutex_unlock(&mmutex);
    		pthread_mutex_unlock(&rmutex);
    		//printf("#DEBUG# queue empty, exiting\n");
    		break;	
    }
    EXITLABEL2: ;
    pthread_exit(NULL);

}

void sut_yield(){

	pthread_mutex_lock(&mmutex);

    queue_insert_tail(&c_q, ptr);
    pthread_mutex_unlock(&mmutex);
	swapcontext(&(runningfunction->threadcontext), &m);
}

void sut_exit(){

	free(runningfunction->threadstack);
	
	activeThreads--;
	//printf("#DEBUG# thread exited\n");
	swapcontext(&(runningfunction->threadcontext), &m);
	
}

void sut_shutdown(){

	//printf("#DEBUG# Shutdown is called\n");
	
	pthread_join(CEXEC, NULL);
	pthread_join(IEXEC, NULL);

	return;
}

void sut_open(char *dest, int port){

	//printf("#DEBUG# we have reached sut_open\n");
	int sockfd =0;	
	//create code
	pthread_mutex_lock(&mmutex);
	iocounter++;	
	iodescarray[iocounter]= runningfunction;	
	pthread_mutex_unlock(&mmutex);

	pthread_mutex_lock(&rmutex);
	iodescarray[iocounter]->dest=(const char*) dest;
	iodescarray[iocounter]->port=(int)port;
	iodescarray[iocounter]->sockfd = sockfd;
	iodescarray[iocounter]->tasktype = 1; //new
	
	struct queue_entry *node = queue_new_node(iodescarray[iocounter]);
    queue_insert_tail(&request_q, node);
	
    //printf("#DEBUG# enqueued sut_open with port: %d\n#DEBUG# enqueued sut_open with dest: %s\n", runningfunction->port, runningfunction->dest);
   
    pthread_mutex_unlock(&rmutex);
    //printf("#DEBUG# instance of sut_open ready to go\n");
    swapcontext(&(iodescarray[iocounter]->threadcontext), &m);

}

void sut_write(char *buf, int size){

	pthread_mutex_lock(&mmutex);
	iocounter++;
	iodescarray[iocounter]= runningfunction;
  	pthread_mutex_unlock(&mmutex);

	pthread_mutex_lock(&rmutex);

	iodescarray[iocounter]->size_msg = size;
	iodescarray[iocounter]->write_msg = buf;
	iodescarray[iocounter]->tasktype = 3;
	
	struct queue_entry *node = queue_new_node(iodescarray[iocounter]);
    queue_insert_tail(&request_q, node);
	
  
    pthread_mutex_unlock(&rmutex);
    //printf("#DEBUG# instance of sut_write ready to go\n");
    swapcontext(&(iodescarray[iocounter]->threadcontext), &m);
}

void sut_close(){

	pthread_mutex_lock(&mmutex);
	iocounter++;
	iodescarray[iocounter]= runningfunction;

	pthread_mutex_unlock(&mmutex);
	pthread_mutex_lock(&rmutex);	
	iodescarray[iocounter]->tasktype = 4;
	struct queue_entry *node = queue_new_node(iodescarray[iocounter]);
    queue_insert_tail(&request_q, node);
	
    
    pthread_mutex_unlock(&rmutex);
    //printf("#DEBUG# instance of sut_close ready to go\n");
    //swapcontext(&(iodescarray[iocounter]->threadcontext), &m);
}

char *sut_read(){

	// pthread_mutex_lock(&mmutex);
	// pthread_mutex_lock(&rmutex);
	// runningfunction->tasktype = 2;
	// runningfunction->recvd_signal = 0;

	// struct queue_entry *node = queue_new_node(runningfunction);
 	// queue_insert_tail(&request_q, node);

 	//  pthread_mutex_unlock(&mmutex);
 	//  pthread_mutex_unlock(&rmutex);

    return "Read has not been implemented!";
}

