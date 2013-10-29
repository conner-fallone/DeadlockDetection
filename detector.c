#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h> 
#include <stdbool.h>
#include <string.h>

typedef struct probeStruct{
	// BlockedProcess:Sender :Receiver
	char blockedProcess[2];
	char senderProcess[2];
	char receiverProcess[2];
} Probe;

Probe probe;
char line[100];
char *owners[20];
char *token;
char *procNum;
int status,counter=0;
pthread_t sender;
pthread_t receiver; 
void *senderThread();
void *receiverThread();
bool isBlocked = false;
bool isDeadlocked = false;
//struct probeStruct probeStruct;

int main(int argc, char *argv[]) 
{
	// Check arguments -> Display usage if incorrect arguments
	if (argc != 3){	
		printf("Arguments: <file> <process #>\n");
		exit(1);
	}
	else{
		//procNum = atoi(argv[2]);
		procNum = argv[2];
		FILE *file = fopen(argv[1],"r");
		if (file == 0){
			printf("Error opening file.\n");
		}else{
			// Opened file successfully.	
			printf("Process num: %s\n",procNum);
			// Analyze config file, read relevant info and initialize state
			// Loop through each line. Every line that contains "own" store in owners array
			// EX: owners[0] = P1 owners[1] = owns owners[2] = r2	
			while (fgets(line,100,file) != NULL){
				printf("Line says: %s\n",line);
				// If the line read contains owns, store it in owners array
				if (strstr(line,"owns") != NULL){
					token = strtok(line, " ");
					while (token != NULL){
						owners[counter] = token;
						token = strtok(NULL, " ");
						counter++;
					}
				}else{
					// We have a request line, see if it is my own process.
					// if it is, set blocked 
					if (strstr(line,procNum)){
						isBlocked = true;
						printf("***** %s is blocking *****\n",procNum);
					}

				}
			}
			fclose(file);
			printf("File closed.\n");			
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
return 1;
}

void *senderThread(void *arg){
	while (isBlocked){
		// Process is blocked
		// Sends a Probe to the process owning the resource it is blocked on
		printf("Sending probe: %s\n", (char*)arg);
		sleep(10);
	}
}
void *receiverThread(void *arg){
	// Check probe 1st and 3rd spots. If equal, deadlocked = true
	if (isBlocked){
		//modifies the Sender and Receiver fields and forwards the message to the process owning the resource it is blocked on. 	
	}else{
		// Discard probe - not blocked
	}
	printf("Testing receiver: %s\n", (char*)arg);
}
