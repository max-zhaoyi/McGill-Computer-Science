#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>

#include "a1_lib.h"

#define BUFSIZE 1024

struct MESSAGE {
		char cmdinput[100];
		float argarray[2];
	};

int main(int argc, char *argv[]) {

//start by checking if input is valid for host ip and port

	if (argc != 3) {
		printf("Please run this command with arguments <host_ip> <host_port>\n");
		fflush(stdout);
		return -1;
	}

	int sockfd;
	char user_input[BUFSIZE] = { 0 };
  	char server_msg[BUFSIZE] = { 0 };

	const char *init_host_ip = argv[1]; 

	char *init_host_port = argv[2];

//connect to server using parsed command arguments; if fails, exit program with error -1
	
	if (connect_to_server(init_host_ip, (uint16_t)atoi(init_host_port), &sockfd) == -1) {
		return -1;
	}
		
	printf(">> ");
	fflush(stdout);

	int loopbreaker = 1; //peace of mind that the program will loop if i want it to, and terminate when i want it to

	while (loopbreaker) {

		memset(user_input, 0, sizeof(user_input));
    	memset(server_msg, 0, sizeof(server_msg));

    	if(fgets(user_input, BUFSIZE,stdin)==NULL) {
    		printf("Error, command not found\n");
    		fflush(stdout);
    		printf(">> ");
			fflush(stdout);
   			continue;
    	};

    	char backupinput[1024];
		strcpy(backupinput,user_input);
    	
		struct MESSAGE newMessage; //= {cmdinput, argarray};

    		
    	char* cmdtemp= strtok(user_input, " ");
    	strcpy(newMessage.cmdinput,cmdtemp);
    		


//check validity of command argument
    	if (	strcmp(newMessage.cmdinput, "add")
    		&& 	strcmp(newMessage.cmdinput, "divide")
    		&& 	strcmp(newMessage.cmdinput, "multiply") 
    		&& 	strcmp(newMessage.cmdinput, "sleep") 
    		&& 	strcmp(newMessage.cmdinput,"factorial")) {

//if command is shutdown or quit, skip parsing (no arguments)
    		if (!strcmp(backupinput, "shutdown\n") || !strcmp(backupinput, "quit\n")){
    			strcpy(newMessage.cmdinput, "shutdown");
    			goto PREPARED;
    		}
//exit terminates the front end right away without sending anything to the server
    		if (!strcmp(backupinput, "exit\n")){
    			loopbreaker = 0;
    			return 0;
    		}
//if command is not valid AND not shutdown, exit or quit, we throw an error and resume taking input
   			printf("Error, command %s not found\n", newMessage.cmdinput);
    		fflush(stdout);
   			printf(">> ");
			fflush(stdout);
   			continue;

    	}
//if command takes in 2 arguments, parse the two arguments
    	if (	!strcmp(newMessage.cmdinput,"add") 
    		|| 	!strcmp(newMessage.cmdinput,"divide" )
    		||	!strcmp(newMessage.cmdinput, "multiply" )){

    		char* pEnd;
    		newMessage.argarray[0] = strtof(strtok(NULL, " "), &pEnd);
    		newMessage.argarray[1] = strtof(strtok(NULL, "\n"),&pEnd);

		} else { //else the command only takes 1 argument
			char* pEnd;
			newMessage.argarray[0] = strtof(strtok(NULL, "\n"),&pEnd);

		}

    		 //done parsing and we have a struct that's ready to be sent

PREPARED:
//sending our message struct that we filled with command and the array of argument floats
    	send_message(sockfd, (char*)&newMessage, sizeof(newMessage));

    	ssize_t byte_count = recv_message(sockfd, server_msg, sizeof(server_msg));
// message recieved, output it
    	if (byte_count <= 0) {
      		break;
    	}
    	printf("%s\n", server_msg);
    	fflush(stdout);
    	printf(">> ");
		fflush(stdout);
		continue;

//if we input shutdown for the server we can close the front end also
		if(!strcmp(backupinput, "shutdown\n") || !strcmp(backupinput, "quit\n")){
			loopbreaker = 0;
			return 0;		
		}

	} //this loop waits until loopbreaker = 0

	return 0;

} // end of main
