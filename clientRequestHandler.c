#include "main.h"
#include "keyValStore.h"
#include "sharedMemory.h"
#include "semaphore.h"
#include "clientRequestHandler.h"

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

#define PUT "PUT"
#define GET "GET"
#define DEL "DEL"
#define QUIT "QUIT"
#define BEG "BEG"
#define END "END"

#define BUFFSIZE 1024

void handleClient(Data* data, const int cfd, int* sem_id, bool* transaction){
	char in[BUFFSIZE];
    int bytes_read;

    while ((bytes_read = read(cfd, in, BUFFSIZE - 1)) > 0) {
        in[bytes_read] = '\0';

        trimMessage(in);
        printf("received message: [%s]\n", in);
        printf("processing input...\n");

        char* command = strtok(in, " ");
        if (command == NULL) continue;

        printf("executing command: [%s]...\n", command);

        char response[BUFFSIZE];

        if (strcmp(command, BEG) == 0) handleBeg(response, sem_id, transaction);
        else if (strcmp(command, END) == 0) handleEnd(response, sem_id, transaction);
        else if (strcmp(command, PUT) == 0) handlePut(data, response, sem_id, transaction);
        else if (strcmp(command, GET) == 0) handleGet(data, response, sem_id, transaction);
        else if (strcmp(command, DEL) == 0) handleDel(data, response, sem_id, transaction);
        else if(strcmp(command, QUIT) == 0) {
            printf("closing connection...!\n");
            return;
        }
        else{
            printf("unknown command [%s].\n", command);
            continue;
        }

        printf("sending message [%s]...\n", response);
        if (send(cfd, response, strlen(response), 0) == -1) {
            perror("send failed!");
        }
        printf("\n");
    }
}


void handlePut(Data* data, char *response, int* sem_id, bool* transaction) {
	if(!*transaction){
		semDown(sem_id, 0);
	}

    char* key = strtok(NULL, " ");
    if (key == NULL) {
        snprintf(response, BUFFSIZE, "> key missing for PUT!\n");
        return;
    }

    char* value  = strtok(NULL, " ");
    if (value == NULL) {
        snprintf(response, BUFFSIZE, "> value missing for PUT!\n");
    } else if (put(data, key, value) > -1) {
        snprintf(response, BUFFSIZE, "> %s:%s:%s successfull!\n", PUT, key, value);
    } else {
        snprintf(response, BUFFSIZE, "> %s:%s:%s failed!\n", PUT, key, value);
    }

	if(!*transaction){
		semUp(sem_id, 0);
	}
}

void handleGet(Data* data, char *response, int* sem_id, bool* transaction) {
    if(!*transaction){
		semDown(sem_id, 0);
	}

	char* key = strtok(NULL, " ");
    if (key == NULL) {
        snprintf(response, BUFFSIZE, "> key missing for GET!\n");
        return;
    }

    char* res = NULL;
    if (get(data, key, &res) > -1) {
        snprintf(response, BUFFSIZE, "> %s:%s:%s\n", GET, key, res);
    } else {
        snprintf(response, BUFFSIZE, "> %s:%s:key_nonexistent\n", GET, key);
    }

	if(!*transaction){
		semUp(sem_id, 0);
	}
}

void handleDel(Data* data, char *response, int* sem_id, bool* transaction) {
    if(!*transaction){
		semDown(sem_id, 0);
	}

    char* key = strtok(NULL, " ");
    if (key == NULL) {
        snprintf(response, BUFFSIZE, "> key missing for DEL!\n");
        return;
    }

    if (del(data, key) > -1) {
        snprintf(response, BUFFSIZE, "> %s:%s:key_deleted\n", GET, key);
    } else {
        snprintf(response, BUFFSIZE, "failed to delete key '%s' or key not found!\n", key);
    }

	if(!*transaction){
		semUp(sem_id, 0);
	}
}

void handleBeg(char *response, int* sem_id, bool* transaction) {
	if(!*transaction){
		semDown(sem_id, 0);
		*transaction = true;
    	printf("%s: transaction started\n", BEG);
        snprintf(response, BUFFSIZE, "> %s:transaction started\n", BEG);
	}
	else {
   	 	printf("%s: unable to start transaction\n", BEG);
    	snprintf(response, BUFFSIZE, "> %s:unable to start transaction\n", BEG);
	}
}

void handleEnd(char *response, int* sem_id, bool* transaction) {
	if(*transaction){
    	semUp(sem_id, 0);
		*transaction = false;
    	printf("%s: transaction terminated\n", END);
    	snprintf(response, BUFFSIZE, "> %s:transaction terminated\n", END);
	}
	else{
    	printf("%s: transaction could not be terminated\n", END);
    	snprintf(response, BUFFSIZE, "> %s:transaction could not be terminated\n", END);
	}
}

void trimMessage(char *in) {
    int end = strlen(in) - 1;
    while (end >= 0 && isspace((unsigned char)in[end])) {
        in[end] = '\0';
        end--;
    }

    char* start = in;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    if (start != in) {
        memmove(in, start, strlen(start) + 1);
    }
}

void cleanUp(int signum){
    shmCleanUp();
    //semCleanUp();
}

