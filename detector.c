#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h> 
#include <stdbool.h>
#include <string.h>

typedef struct probeStruct{
	// BlockedProcess : Sender : Receiver
	char *blockedProcess;
	char *senderProcess;
	char *receiverProcess;
} Probe;

Probe probe;
char line[100];
char *owners[20];
char *requesters[20];
char *token;
char *procNum;
char *resource;
char *resourceOwner;
int i,status,counter,counter2;
pthread_t sender;
pthread_t receiver; 
void *senderThread();
void *receiverThread();
void findOwner();
bool isBlocked = false;
bool isDeadlocked = false;

int main(int argc, char *argv[]) 
{
	// Check arguments -> Display usage if incorrect arguments
	if (argc != 3){	
		printf("Arguments: <file> <process #>\n");
		exit(1);
	}
	else{
		// Get process number argument
		procNum = argv[2];

		// Open configuration file
		FILE *file = fopen(argv[1],"r");
		if (file == 0){
			printf("Error opening file.\n");
		}else{
			// Opened file successfully.	
			// Store configuration file
			while (fgets(line,100,file) != NULL){
				printf("Line: %s\n",line);
				// If the line read contains owns, store it in owners array
				if (strstr(line,"owns") != NULL){
					
					token = strtok(line, " ");
					while (token != NULL){
						// Don't need to store 'owns'
						if (!strcmp(token,"owns")){
							token = strtok(NULL, " ");
							continue;
						}
						// Allocate space and copy token into array
						owners[counter] = malloc(strlen(token) + 1);
						strcpy(owners[counter], token);
						token = strtok(NULL, " ");
						counter++;
					}
				}else{
					// We have a request line, see if it is my own proces, block if it is
					if (strstr(line,procNum)){

						token = strtok(line, " ");
						while (token != NULL){
							// Don't need to store 'requests'
							if (!strcmp(token,"requests")){
								token = strtok(NULL, " ");
								continue;
							}
							// Allocate space and copy token into array
							requesters[counter2] = malloc(strlen(token) + 1);
							strcpy(requesters[counter2], token);
							token = strtok(NULL, " ");
							counter2++;
						}	
						isBlocked = true;	
						printf("***** %s is blocking *****\n",procNum);
					}

				}
			}
			printf("**** Done reading configuration ****\n");
			fclose(file);	
			
			// If process is blocked, find the owner of resource i'm blocked on and form probe
			if (isBlocked)
				findOwner();	
		}	
	}

	// Create sender thread
	if ((status = pthread_create(&sender,NULL,senderThread,"Testing create")) != 0){
		fprintf (stderr, "Error creating thread - Status: %d: %s\n", status, strerror(status));
		exit (1);
	}

	// Create receiver thread
	if ((status = pthread_create(&receiver,NULL,receiverThread,"Testing create")) != 0){
		fprintf (stderr, "Error creating thread - Status: %d: %s\n", status, strerror(status));
		exit (1);
	}

	// Main thread - loop while not deadlocked
	while (!isDeadlocked) {
		printf("Looping\n");
		sleep(1);
	}

	printf("**** System is deadlocked ****");
return 1;
}

void *senderThread(void *arg){
	while (isBlocked){
		// Process is blocked
		// Sends a Probe to the process owning the resource it is blocked on
		sleep(5);
		printf("**** Sending probe: %s:%s:%s ****\n", probe.blockedProcess, probe.senderProcess, probe.receiverProcess);
	}
}
void *receiverThread(void *arg){
	// Check probe 1st and 3rd spots. If equal, deadlocked = true
	if (isBlocked){
		// Struct current probe = probe read
		findOwner();
		//modifies the Sender and Receiver fields and forwards the message to the process owning the resource it is blocked on. 	
	}else{
		probe.blockedProcess = NULL;
		probe.senderProcess = NULL;
		probe.receiverProcess = NULL;
	}
	printf("Testing receiver: %s\n", (char*)arg);
}

void findOwner(){
	// Search through requesters. Find the resource i'm blocked on
	while (1){
		if (!strcmp(requesters[i],procNum)){
			printf("%s wants %s\n",requesters[i],requesters[i+1]);
			resource = requesters[i+1];
			break;
		}
		i++;
	}
	i = 0;
	// Search through owners. Find the owner of the resource i'm blocked on
	while (1){
		if (!strcmp(owners[i],resource)){
			printf("%s owns %s\n",owners[i-1],owners[i]);
			resourceOwner = owners[i-1];
			break;
		}
		i++;	
	}

	// Form probe
	probe.blockedProcess = procNum;
	probe.senderProcess = procNum;
	probe.receiverProcess = resourceOwner;
	printf("%s:%s:%s\n",probe.blockedProcess,probe.senderProcess,probe.receiverProcess);
	i = 0;
}
