#include "shared_memory.h"

#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>

Data *seg_global;
int shm_id;

int shmInit(Data** seg) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &shmCleanUp;
    sigaction(SIGINT, &sa, NULL);

    shm_id = shmget(IPC_PRIVATE, (ARRAY_SIZE * sizeof(Data)), IPC_CREAT | 0600);
    if (shm_id < 0) {
        perror("shmget failed");
        return -1;
    }

    void *temp = shmat(shm_id, NULL, 0);
    if (temp == (void *) -1) {
        perror("shmat failed");
        return -1;
    }

    *seg = (Data *)temp;
    seg_global = *seg;

    return 0;
}

void shmCleanUp(int signum) {
    if (seg_global != NULL) {
        shmdt(seg_global);
        seg_global = NULL;
    }

    if (shm_id > 0) shmctl(shm_id, IPC_RMID, NULL);

    exit(0);
}