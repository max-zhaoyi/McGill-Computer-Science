#ifndef __SUT_H__
#define __SUT_H__
#include <stdbool.h>
#include <ucontext.h>
#include <stdint.h>

#define MAX_THREADS 10
#define THREAD_STACK_SIZE                  1024*64

typedef void (*sut_task_f)();


typedef struct __threaddesc
{
	int threadid;
	char *threadstack;
	void *threadfunc; //
	ucontext_t threadcontext;
	//below are io specific
	int tasktype; //1 for open 2 for read 3 for write 4 for close
	int sockfd;
	const char *dest;
	uint16_t port;
	char* write_msg;
	char* recvd_msg;
	int size_msg;
	int isrunning;
	int recvd_signal;
	int socketopen;
} threaddesc;

void sut_init();
bool sut_create(sut_task_f fn);
void sut_yield();
void sut_exit();
void sut_open(char *dest, int port);
void sut_write(char *buf, int size);
void sut_close();
char *sut_read();
void sut_shutdown();


#endif
