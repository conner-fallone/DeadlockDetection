/******************************************************
 * Author: Conner Fallone
 * CS-452 Fall 2013
 * Deadlock Detection
 ******************************************************/

#include <stdio.h> 
#include <stdlib.h>
#include <pthread.h> 
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

pthread_t sender;
pthread_t receiver; 
char line[100];
char *owners[20];
char *requesters[20];
char *probes[3];
char *token;
char *procNum;
char *resource;
char *resourceOwner;
int i,status,ownersCounter,requesterCounter,fd;
bool isBlocked = false;
bool isDeadlocked = false;
void *senderThread();
void *receiverThread();
void findOwner();

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
				printf("Parsing Line: %s\n",line);
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
						owners[ownersCounter] = malloc(strlen(token) + 1);
						strcpy(owners[ownersCounter], token);
						token = strtok(NULL, " ");
						ownersCounter++;
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
							requesters[requesterCounter] = malloc(strlen(token) + 1);
							strcpy(requesters[requesterCounter], token);
							token = strtok(NULL, " ");
							requesterCounter++;
						}	
						isBlocked = true;		
					}

				}
			}
			printf("**** Done reading configuration ****\n");
			fclose(file);	
			
			// If process is blocked, find the owner of resource i'm blocked on and form probe
			if (isBlocked){
				findOwner();
			}
		}	
	}
	// Create sender thread
	if ((status = pthread_create(&sender,NULL,senderThread,NULL)) != 0){
		fprintf (stderr, "Error creating thread - Status: %d: %s\n", status, strerror(status));
		exit (1);
	}

	// Create receiver thread
	if ((status = pthread_create(&receiver,NULL,receiverThread,NULL)) != 0){
		fprintf (stderr, "Error creating thread - Status: %d: %s\n", status, strerror(status));
		exit (1);
	}

	// Main thread - loop while not deadlocked
	while (!isDeadlocked) {
		// do nothing while not deadlocked	
	}

	printf("**** System is deadlocked ****\n");
return 1;
}

/*******************************************************
 * Sending thread, sends probes every 10 seconds.
 *******************************************************/
void *senderThread(){
	while (isBlocked){
		// Sends a Probe to the process owning the resource it is blocked on
		fd = open(resourceOwner, O_WRONLY);
		printf("%s is Writing %s:%s:%s\n",procNum,probes[0], probes[1], probes[2]);	
		write(fd, probes[0], sizeof(char) * 2);
		write(fd, probes[1], sizeof(char) * 2);
		write(fd, probes[2], sizeof(char) * 2);
		close(fd);
		printf("%s wrote to the pipe.\n",procNum);
		sleep(10);
	}
return NULL;
}

/*******************************************************
 * Receiving thread, constantly looking for a probe
 * that wants to contact the process.
 *******************************************************/
void *receiverThread(){
	while (1){
		// Listen for probe - Look for any pipe with a name equivalent to process number
		// this means there is a process that wishes to contact me
		fd = open(procNum, O_RDONLY);
		if (fd < 0){
			// There is no pipe with my name. Noone wants to contact me
		}else{
			if (isBlocked){
				read(fd,probes[0],sizeof(char) * 2);
				read(fd,probes[1],sizeof(char) * 2);
				read(fd,probes[2],sizeof(char) * 2);
				printf("%s has read %s:%s:%s\n",procNum,probes[0],probes[1],probes[2]);
				// Check probe blocked and receiving process. If same = deadlocked
				if (!strcmp(probes[0],probes[2])){
					printf("%s detected deadlock because %s = %s\n",procNum,probes[0],probes[2]);
					close (fd);
					unlink (procNum);
					free(requesters[0]);
					isDeadlocked = true;
				}		
				strcpy(probes[1],procNum);
				strcpy(probes[2],resourceOwner);
			}else{
				printf("%s is not blocked and received a probe. discarding.\n",procNum);
			}
		}
	sleep(5);	
	}
return NULL;
}

/*******************************************************
 * Called when the process is blocking. Find the owner
 * of the resource the process is blocked on, and forms
 * the probe.
 *******************************************************/
void findOwner(){
	printf("***** %s is blocking *****\n",procNum);
	// Search through requesters. Find the resource i'm blocked on
	while (1){
		if (!strcmp(requesters[i],procNum)){
			resource = requesters[i+1];
			break;
		}
		i++;
	}
	i = 0;
	// Search through owners. Find the owner of the resource i'm blocked on
	while (1){
		if (!strcmp(owners[i],resource)){
			resourceOwner = owners[i-1];
			break;
		}
		i++;	
	}
	// Create pipe/file
	mkfifo(resourceOwner,0666);
	printf("%s created the pipe %s\n",procNum,resourceOwner);	
	// Form probe for blocked process
	probes[0] = malloc(strlen(procNum) + 1);
	strcpy(probes[0], procNum);
	probes[1] = malloc(strlen(procNum) +1);
	strcpy(probes[1], procNum);
	probes[2] = malloc(strlen(resourceOwner) + 1);
	strcpy(probes[2], resourceOwner);
	i = 0;
}
