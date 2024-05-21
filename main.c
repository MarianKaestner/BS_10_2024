#include "main.h"
#include "keyValStore.h"
#include "sharedMemory.h"
#include "semaphore.h"
#include "clientRequestHandler.h"

#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define TRUE 1
#define PORT 5678

int main(void) {
    Data* data;
    bool transaction = false;
    int rfd, cfd;

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &cleanUp;
    sigaction(SIGINT, &sa, NULL);

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

    int sem_id;
    if(semInit(&sem_id, 1) < 0) {
        fprintf(stderr, "failed to initialize semaphore\n");
        exit(1);
    }

    if(shmInit(&data) < 0) {
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
            handleClient(data, cfd, &sem_id, &transaction);
            exit(0);
        }
        else {
            close(cfd);
        }
    }
    close(rfd);
    return 0;
}


