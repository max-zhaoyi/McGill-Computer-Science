#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "a1_lib.h"
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h> 

#define BUFSIZE   1024

struct MESSAGE {
    char cmdinput[100];
    float argarray[2];
  };


int addInts(float x, float y){
  return (int)(x+y);
}

float divideFloats(float x, float y){
//already handled division by 0 in lookup table
  return x/y;  
  //printf("%.6f\n", z)
}

int multiplyInts(float x, float y){
  return (int)x*y;
}

uint64_t factorialOfInt(float x){

  uint64_t counter = 1;
  uint64_t facttemp = (uint64_t)x;

  if (facttemp <= (uint64_t)0) {
    return (uint64_t)0;
  }

  while (facttemp>(uint64_t)1) {
    counter=facttemp*counter;
    facttemp=facttemp-(uint64_t)1;
  }

  return counter;
}

void sleepy(int x){
  sleep(x);
  return;
}

int main(int argc, char *argv[]) {

  if (argc != 3) {
    printf("Please run this command with arguments <host_ip> <host_port>\n");
    fflush(stdout);
    return -1;
  }

  
   
  const char *init_host_ip = argv[1]; 
  char *init_host_port = argv[2];

  int sockfd, clientfd;
  char msg[BUFSIZE];
  int running = 1;

  if (create_server(init_host_ip, (uint16_t)atoi(init_host_port), &sockfd) < 0) {
    fprintf(stderr, "Server was not created successfully\n");
    return -1;
  }
  printf("server listening on <%s>:<%s>\n", init_host_ip, init_host_port);


  int checker =0;

  pid_t pidArray[6];
  int pidCounter = 0;

  pidArray[pidCounter] = getpid(); //parent process id inside of pidArray[0];
  pidCounter++;
  
  if (fork()==0) {

    pidArray[pidCounter] = getpid();
    pidCounter++;

    if (fork()==0){

      pidArray[pidCounter] = getpid();
      pidCounter++;

      if (fork()==0) {

        pidArray[pidCounter] = getpid();
        pidCounter++;
     
      }
      if (fork() ==0){
        checker =1;
      }
      pidArray[pidCounter] = getpid();
      pidCounter++;


    }
   
  } //we now have 6 pids

//debug
//   if (checker == 1){
// for (int g =0; g<6; g++){
//   printf("process id: %d\n", pidArray[g]);
// }
// checker = 0;
//
//}
if (fork()==0){
  if (accept_connection(sockfd, &clientfd) < 0) {
    fprintf(stderr, "Connection between the server and the client was unsuccessful\n");
    return -1;
  }

  //while (strcmp(msg, "quit\n")) {
  int loopbreaker = 1; //peace of mind that the program will loop if i want it to, and terminate when i want it to

  while(loopbreaker){
    memset(msg, 0, sizeof(msg));
    ssize_t byte_count = recv_message(clientfd, msg, BUFSIZE);

    if (byte_count <= 0) {
      break;
    }

    struct MESSAGE* recvdmsg = (struct MESSAGE*)msg;


    char* cmd = recvdmsg->cmdinput;
    char *tmpfloatstring;

    printf("cmd: %s\n", cmd);

    float arg1 = (recvdmsg->argarray[0]);
    float arg2 = (recvdmsg->argarray[1]);

    void* result = NULL;
    int result_type = 0;

   //if result_type = 1, then we know it's an int
   //               = 2, then we know it's a float
   //               = 3, then we know it's a uint64_t
   //               = 4, then we know it's a string 
   //               = 5, then we know it's void 

   //lookup table:
    int i;
    float f;
    uint64_t t;
    char * c;


    if (!strcmp(cmd, "add")) {
      i = addInts(arg1, arg2); 
      result = &i;
      result_type = 1;

    } else if (!strcmp(cmd, "multiply")){

      i = multiplyInts(arg1, arg2); 
      result = &i;
      result_type = 1;

    } else if (!strcmp(cmd, "divide")){

      if (arg2 == 0) { 

        char* errorRes = "Error, division by zero"; 
        result = errorRes; result_type = 4;

      } else{

        f = divideFloats(arg1, arg2);
        result = &f;
        result_type = 2;

      }
      
    } else if (!strcmp(cmd, "factorial")){
        
        t = factorialOfInt(arg1); 
        result = &t;
        result_type = 3;
      
    } else if (!strcmp(cmd, "sleep")){

      sleepy(arg1); 
      
      result_type = 5;
      
    } else if (!strcmp(cmd, "shutdown")){

      loopbreaker = 0;
      for (int b=0; b<6;b++){

        kill(pidArray[b], SIGTERM);
      }
      return 0;
    }

    char tmpStr[1024];
    //printf("Client: %s\n", msg);
    if (result_type == 1) { //result type is int

      sprintf(tmpStr, "%d", *(int *)result);
      send_message(clientfd, tmpStr, strlen(tmpStr));

    } else if (result_type == 2){ //result type is float

      sprintf(tmpStr, "%.6f", *(float *)result);
      send_message(clientfd, tmpStr, strlen(tmpStr));

    } else if (result_type == 3){ //result is uint_64

      sprintf(tmpStr, "%"PRIu64, *(uint64_t *)result);
      send_message(clientfd, tmpStr, strlen(tmpStr));

    } else if (result_type == 4) { //specific string, error division by zero
      sprintf(tmpStr, "%s", (char*)result);
      send_message(clientfd, tmpStr, strlen(tmpStr));

    } else if (result_type == 5) { //case of sleep

      sprintf(tmpStr, "done sleeping!");
      send_message(clientfd, tmpStr, strlen(tmpStr));

    } else {  //hopefully dead code (debug feature)
      strcpy(tmpStr,"Something went wrong!");
      send_message(clientfd, tmpStr, strlen(tmpStr));
    }
    
  }
    // while (wait(NULL) != -1 ||errno != ECHILD){
    //   return 0;
    // }
   return 0;
  }// end of main while loop
  return 0;

}