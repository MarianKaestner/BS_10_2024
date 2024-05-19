#include "main.h"
#include "keyValStore.h"
#include "shared_memory.h"

#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>

int sem_id;
struct sembuf enter, leave;

int semInit() {
    sem_id = semget (IPC_PRIVATE, 1, IPC_CREAT|0644);
    if (sem_id == -1) {
        return -1;
    }

    unsigned short initialValue[1] = {1};
    if(semctl(sem_id, 0, SETALL, initialValue) == -1) {
        perror("semctl SETALL failed");
        return -1;
    }

    enter.sem_num = leave.sem_num = 0;
    enter.sem_flg = leave.sem_flg = SEM_UNDO;
    enter.sem_op = -1;
    leave.sem_op = 1;

    return 0;
}

int semUp(){
    return semop(sem_id, &leave, 1);
}

int semDown(){
    return  semop(sem_id, &enter, 1);
}

int semCleanUp(){
    if (sem_id > 0) semctl(sem_id, 0, IPC_RMID);
    return 0;
}