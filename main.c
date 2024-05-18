#include "main.h"
#include "keyValStore.h"

#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define BUFFSIZE 1024
#define SEGSIZE (ARRAY_SIZE * sizeof(Data))
#define TRUE 1
#define PORT 5678

#define PUT "PUT"
#define GET "GET"
#define DEL "DEL"
#define QUIT "QUIT"

key_t key;
int shm_id;

void cleanUp(int signum) {
    if (data != NULL) {
        shmdt(data);
        data = NULL;
    }

    if (shm_id > 0) {
        shmctl(shm_id, IPC_RMID, NULL);
    }

    exit(0);
}

int initShm() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &cleanUp;
    sigaction(SIGINT, &sa, NULL);

    key = ftok("/tmp/ipc.tmp", 'R');
    if (key == -1) {
        perror("ftok failed");
        exit(1);
    }

    shm_id = shmget(key, SEGSIZE, IPC_CREAT | 0600);
    if (shm_id < 0) {
        perror("shmget failed");
        return -1;
    }

    void *temp = shmat(shm_id, NULL, 0);
    if (temp == (void *) -1) {
        perror("shmat failed");
        return -1;
    }

    data = (Data *)temp;
    return 0;
}

void putKey(char* command, char* key, char* value, char *response) {
    value = strtok(NULL, " ");
    if (value == NULL) {
        snprintf(response, BUFFSIZE, "> value missing for PUT!\n");
    } else if (put(key, value) > -1) {
        snprintf(response, BUFFSIZE, "> %s:%s:%s successfull!\n",command, key, value);
    } else {
        snprintf(response, BUFFSIZE, "> %s:%s:%s failed!\n",command, key, value);
    }
}

void getKey(char* command, char* key, char *response) {
    char* res = NULL;
    if (get(key, &res) > -1) {
        snprintf(response, BUFFSIZE, "> %s:%s:%s\n", command, key, res);
    } else {
        snprintf(response, BUFFSIZE, "> %s:%s:key_nonexistent\n", command, key);
    }
}

void delKey(char* command, char* key, char *response) {
    if (del(key) > -1) {
        snprintf(response, BUFFSIZE, "> %s:%s:key_deleted\n", command, key);
    } else {
        snprintf(response, BUFFSIZE, "failed to delete key '%s' or key not found!\n", key);
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

void processClientCommands(const int cfd) {
    char in[BUFFSIZE];
    int bytes_read;

    while ((bytes_read = read(cfd, in, BUFFSIZE - 1)) > 0) {
        in[bytes_read] = '\0';
        printf("received command: [%s]\n", in);
        printf("processing input...\n");

        trimMessage(in);

        char* command = strtok(in, " ");
        if (command == NULL) continue;

        char* key = strtok(NULL, " ");
        if (key == NULL) continue;

        char* value = NULL;
        char response[BUFFSIZE];

        if (strcmp(command, PUT) == 0) putKey(command, key, value, response);
        else if (strcmp(command, GET) == 0) getKey(command, key, response);
        else if (strcmp(command, DEL) == 0) delKey(command, key, response);
        else if(strcmp(command, QUIT) == 0) {
            printf("connection closed!\n");
            break;
        }

        printf("sending message [%s]...\n", response);
        if (send(cfd, response, strlen(response), 0) == -1) {
            perror("send failed!");
        }
    }
}


int main(void) {

    int rfd, cfd;

    struct sockaddr_in client;
    socklen_t client_len;

    printf("creating socket...\n");
    rfd = socket(AF_INET, SOCK_STREAM, 0);
    if (rfd < 0 ){
        fprintf(stderr, "socket creation failed!\n");
        exit(-1);
    }


    int option = 1;
    setsockopt(rfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &option, sizeof(int));


    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    printf("binding socket...\n");
    int brt = bind(rfd, (struct sockaddr *) &server, sizeof(server));
    if (brt < 0 ){
        fprintf(stderr, "binding failed\n");
        exit(-1);
    }


    printf("listening...\n");
    int lrt = listen(rfd, 5);
    if (lrt < 0 ){
        fprintf(stderr, "listening failed!\n");
        exit(-1);
    }

    if(initShm() < 0) {
        fprintf(stderr, "failed to initialize shared memory\n");
        exit(1);
    }

    printf("Server running on port %d.\n", PORT);

    while (TRUE) {
        printf("waiting for client...\n");

        client_len = sizeof(client);
        cfd = accept(rfd, (struct sockaddr *) &client, &client_len);
        if (cfd < 0) {
            perror("accept failed!");
            continue;
        }

        printf("client accepted!\n");
        pid_t pid = fork();

        if (pid == 0) {
            close(rfd);
            processClientCommands(cfd);
            exit(0);
        }
        else {
            close(cfd);
        }
    }

    close(rfd);
    return 0;
}
