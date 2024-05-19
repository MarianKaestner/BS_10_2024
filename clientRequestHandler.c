#include "main.h"
#include "keyValStore.h"
#include "shared_memory.h"
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

void handleClient(Data* data, const int cfd){
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

        if (strcmp(command, PUT) == 0) handlePut(data, command, response);
        else if (strcmp(command, GET) == 0) handleGet(data, command, response);
        else if (strcmp(command, DEL) == 0) handleDel(data, command, response);
        else if (strcmp(command, BEG) == 0) handleBeg(command, response);
        else if (strcmp(command, END) == 0) handleEnd(command, response);
        else if(strcmp(command, QUIT) == 0) {
            printf("closing connection...!\n");
            return;
        }
        else {
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


void handlePut(Data* data, const char* command, char *response) {
    char* key = strtok(NULL, " ");
    if (key == NULL) {
        snprintf(response, BUFFSIZE, "> key missing for PUT!\n");
        return;
    }

    char* value  = strtok(NULL, " ");
    if (value == NULL) {
        snprintf(response, BUFFSIZE, "> value missing for PUT!\n");
    } else if (put(data, key, value) > -1) {
        snprintf(response, BUFFSIZE, "> %s:%s:%s successfull!\n",command, key, value);
    } else {
        snprintf(response, BUFFSIZE, "> %s:%s:%s failed!\n",command, key, value);
    }
}

void handleGet(Data* data, const char* command, char *response) {
    char* key = strtok(NULL, " ");
    if (key == NULL) {
        snprintf(response, BUFFSIZE, "> key missing for GET!\n");
        return;
    }

    char* res = NULL;
    if (get(data, key, &res) > -1) {
        snprintf(response, BUFFSIZE, "> %s:%s:%s\n", command, key, res);
    } else {
        snprintf(response, BUFFSIZE, "> %s:%s:key_nonexistent\n", command, key);
    }
}

void handleDel(Data* data, const char* command, char *response) {
    char* key = strtok(NULL, " ");
    if (key == NULL) {
        snprintf(response, BUFFSIZE, "> key missing for DEL!\n");
        return;
    }

    if (del(data, key) > -1) {
        snprintf(response, BUFFSIZE, "> %s:%s:key_deleted\n", command, key);
    } else {
        snprintf(response, BUFFSIZE, "failed to delete key '%s' or key not found!\n", key);
    }
}

void handleBeg(const char* command, char *response) {
    printf("BEG operation requested\n");
    if (semDown() == 0) {
        snprintf(response, BUFFSIZE, "> BEG operation successful.\n");
        printf("BEG operation successful.\n");
    } else {
        snprintf(response, BUFFSIZE, "> BEG operation failed\n");
        printf("BEG operation failed\n");
    }
}

void handleEnd(const char* command, char *response) {
    printf("END operation requested\n");
    if (semUp() == 0) {
        snprintf(response, BUFFSIZE, "> END operation successful.\n");
        printf("END operation successful.\n");
    } else {
        snprintf(response, BUFFSIZE, "> END operation failed\n");
        printf("END operation failed\n");
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

