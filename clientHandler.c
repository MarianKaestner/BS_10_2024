#include "semaphore.h"
#include "clientHandler.h"
#include "msgQueue.h"

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#define BUFFSIZE 1024

const char* CmdTypeNames[] = {
    "PUT",
    "GET",
    "DEL",
    "BEG",
    "END",
    "SUB",
    "QUIT"
};

void handleClient(KeyVal* keyValStore, const int cfd, int* sem_id, bool* transaction, SubPool* pool){
    printf("initializing message queue...\n");
    int msid;
    if(creMsgQueue(&msid)) {
        fprintf(stderr, "failed to initialize message queue\n");
        exit(1);
    }

    pid_t pid = fork();

    if(pid < 0) {
        fprintf(stderr, "fork failed\n");
        exit(-1);
    }

    if (pid == 0) {
        handleMsgReceive(cfd, pool, msid);
    }
    else {
        handleConnection(keyValStore, cfd, sem_id, transaction, pool, msid);
    }
}

void handleMsgReceive(int cfd, SubPool* pool, int msid) {
    while(true) {
        Msg msg;
        printf("Warte auf Nachrichten in der Warteschlange mit msid: %d...\n", msid);

        if(msgReceive(msid, &msg)) {
            fprintf(stderr, "Fehler beim Empfangen einer Nachricht aus der Warteschlange: %d\n", msid);
            exit(-1);
        }

        printf("Nachricht empfangen: CmdType: %d, Key: %s, Value: %s\n", msg.cmdType, msg.key, msg.value);

        char response[BUFFSIZE];
        snprintf(response, BUFFSIZE, "> %s:%s:%s\n", CmdTypeNames[msg.cmdType], msg.key, msg.value);

        printf("Sende Antwort: %s\n", response);
        sendMessage(cfd, response);

    }
}

void handleConnection(KeyVal* keyValStore, const int cfd, int* sem_id, bool* transaction, SubPool* pool, int msid) {
    char in[BUFFSIZE];
    int bytes_read;

    while ((bytes_read = read(cfd, in, BUFFSIZE - 1)) > 0) {
        in[bytes_read] = '\0';

        trimMessage(in);
        printf("received message: [%s]\n", in);
        printf("processing input...\n");

        char* in_r = in;
        char* cmd = strtok_r(in_r, " ", &in_r);
        if (cmd == NULL) continue;

        printf("executing command: [%s]...\n", cmd);

        char response[BUFFSIZE];

        if (strcmp(cmd, CmdTypeNames[PUT]) == 0) handlePut(keyValStore, pool, msid, response, sem_id, transaction, in_r);
        else if (strcmp(cmd, CmdTypeNames[GET]) == 0) handleGet(keyValStore, response, sem_id, transaction, in_r);
        else if (strcmp(cmd, CmdTypeNames[DEL]) == 0) handleDel(keyValStore, pool, msid, response, sem_id, transaction, in_r);
        else if (strcmp(cmd, CmdTypeNames[BEG]) == 0) handleBeg(response, sem_id, transaction);
        else if (strcmp(cmd, CmdTypeNames[END]) == 0) handleEnd(response, sem_id, transaction);
        else if (strcmp(cmd, CmdTypeNames[SUB]) == 0) {
            printf("SUB!!!\n");
            handleSub(keyValStore, pool, msid, response, in_r);
        }
        else if(strcmp(cmd, CmdTypeNames[QUIT]) == 0) {
            printf("closing connection...!\n");
            return;
        }
        else{
            printf("unknown command [%s].\n", cmd);
            continue;
        }

        sendMessage(cfd, response);
    }
}


void handlePut(KeyVal* keyValStore, SubPool* pool, int msid,  char *response, int* sem_id, bool* transaction, char* in_r) {
	if(!*transaction){
		semDown(sem_id, 0);
	}

    char* key = strtok_r(NULL, " ", &in_r);
    if (key == NULL) {
        snprintf(response, BUFFSIZE, "> key missing for PUT!\n");
        return;
    }

    char* value  = strtok_r(NULL, " ", &in_r);
    if (value == NULL) {
        snprintf(response, BUFFSIZE, "> res missing for %s!\n", CmdTypeNames[PUT]);
    } else if (put(keyValStore, key, value) > -1) {
        snprintf(response, BUFFSIZE, "> %s:%s:%s successfull!\n", CmdTypeNames[PUT], key, value);
        sendSubMessage(pool, msid,  PUT, key, value);
    } else {
        snprintf(response, BUFFSIZE, "> %s:%s:%s failed!\n", CmdTypeNames[PUT], key, value);
    }

	if(!*transaction){
		semUp(sem_id, 0);
	}
}

void handleGet(KeyVal* keyValStore, char *response, int* sem_id, bool* transaction, char* in_r) {
    if(!*transaction){
		semDown(sem_id, 0);
	}

    char* key = strtok_r(NULL, " ", &in_r);
    if (key == NULL) {
        snprintf(response, BUFFSIZE, "> key missing for %s!\n", CmdTypeNames[GET]);
        return;
    }

    char* res = NULL;
    if (get(keyValStore, key, &res) > -1) {
        snprintf(response, BUFFSIZE, "> %s:%s:%s\n", CmdTypeNames[GET], key, res);
    } else {
        snprintf(response, BUFFSIZE, "> %s:%s:key_nonexistent\n", CmdTypeNames[GET], key);
    }

	if(!*transaction){
		semUp(sem_id, 0);
	}
}

void handleDel(KeyVal* keyValStore, SubPool* pool, int msid, char *response, int* sem_id, bool* transaction, char* in_r) {
    if(!*transaction){
		semDown(sem_id, 0);
	}

    char* key = strtok_r(NULL, " ", &in_r);
    if (key == NULL) {
        snprintf(response, BUFFSIZE, "> key missing for %s!\n", CmdTypeNames[DEL]);
        return;
    }

    if (del(keyValStore, key) > -1) {
        snprintf(response, BUFFSIZE, "> %s:%s:key_deleted\n", CmdTypeNames[DEL], key);
        sendSubMessage(pool, msid, DEL, key, "key_deleted");
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
    	printf("%s: transaction started\n", CmdTypeNames[BEG]);
        snprintf(response, BUFFSIZE, "> %s:transaction started\n", CmdTypeNames[BEG]);
	}
	else {
   	 	printf("%s: unable to start transaction\n", CmdTypeNames[BEG]);
    	snprintf(response, BUFFSIZE, "> %s:unable to start transaction\n", CmdTypeNames[BEG]);
	}
}

void handleEnd(char *response, int* sem_id, bool* transaction) {
	if(*transaction){
    	semUp(sem_id, 0);
		*transaction = false;
    	printf("%s: transaction terminated\n", CmdTypeNames[END]);
    	snprintf(response, BUFFSIZE, "> %s:transaction terminated\n", CmdTypeNames[END]);
	}
	else{
    	printf("%s: transaction could not be terminated\n", CmdTypeNames[END]);
    	snprintf(response, BUFFSIZE, "> %s:transaction could not be terminated\n", CmdTypeNames[END]);
	}
}

void handleSub(KeyVal* keyValStore, SubPool* pool, int msid, char* response, char* in_r) {
    char* key = strtok_r(NULL, " ", &in_r);
    if (key == NULL) {
        snprintf(response, BUFFSIZE, "> key missing for %s!\n", CmdTypeNames[SUB]);
        return;
    }

    char* res = NULL;
    if(sub(pool, key, msid) > -1 && get(keyValStore, key, &res) > -1) {
        snprintf(response, BUFFSIZE, "> %s:%s\n", CmdTypeNames[SUB], key);
    }
    else {
        snprintf(response, BUFFSIZE, "> subscription to key '%s' failed", key);
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

void sendSubMessage(SubPool* pool, int msid,  CmdType cmdType, char* key, char* value) {
    for(int i = 0; i < pool->keyCount; i++) {
        if(strncmp(pool->subs[i].key, key, KEY_SIZE) == 0) {
            for(int j = 0; j < pool->subs[i].subCount; j++) {
                if(pool->subs[i].msids[j] != msid) {
                    msgSend(pool->subs[i].msids[j], cmdType, key, value);
                }
            }
        }
    }
}

int sendMessage(int cfd, char* message) {
    printf("sending message [%s]...\n", message);
    if (send(cfd, message, strlen(message), 0) == -1) {
        perror("send failed!");
        return -1;
    }
    printf("\n");
    return 0;
}

void cleanUp(int signum){
    //shmCleanUp();
    //semCleanUp();
}

