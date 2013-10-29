#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h> 
#include <stdbool.h>
#include <string.h>

char line[100];
char *probe;
char *owners[20];
char *token;
int procNum,status,counter=0;
pthread_t sender;
pthread_t receiver; 
void *senderThread();
void *receiverThread();
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
		procNum = atoi(argv[2]);
		FILE *file = fopen(argv[1],"r");
		if (file == 0){
			printf("Error opening file.\n");
		}else{
			// Opened file successfully.	
			printf("Process num: %d\n",procNum);
			// Analyze config file, read relevant info and initialize state
			// Loop through each line. Every line that contains "own" store in owners array
			// EX: owners[0] = P1 owners[1] = owns owners[2] = r2	
			while (fgets(line,100,file) != NULL){
				printf("Line says: %s\n",line);
				// If the line read contains owns, store it in owners array
				if (strstr(line,"owns") != NULL){
					token = strtok(line, " ");
					while (token != NULL){
						printf("Token is: %s\n",token);
						owners[counter] = token;
						printf("owners[%d]: %s\n",counter,owners[counter]);
						token = strtok(NULL, " ");
						counter++;
					}
				}else{
				// We have a request line, see if it is my own process.
				// if it is, set blocked 

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
	while (!isBlocked){
		printf("Sending probe: %s\n", (char*)arg);
		sleep(10);
	}
}
void *receiverThread(void *arg){
	printf("Testing receiver: %s\n", (char*)arg);
}
