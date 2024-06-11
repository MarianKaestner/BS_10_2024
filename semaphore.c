#include "semaphore.h"
#include "main.h"
#include "keyValStore.h"

#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>

int semInit(int* sem_id, int count) {
    *sem_id = semget(IPC_PRIVATE, count, IPC_CREAT | 0666);
    if (*sem_id == -1) {
        perror("semget failed");
        return -1;
    }

    unsigned short initialValues[count];
	for(int i = 0; i < count; i++) {
       initialValues[i] = 1;
   }
    if(semctl(*sem_id, 0, SETALL, initialValues) == -1) {
        perror("semctl SETALL failed");
        return -1;
    }

    return 0;
}

int semUp(int* sem_id, int sem_num){
    struct sembuf op;
    op.sem_num = sem_num;
    op.sem_op = 1;
    op.sem_flg = SEM_UNDO;
    return semop(*sem_id, &op, 1);
}

int semDown(int* sem_id, int sem_num){
    struct sembuf op;
    op.sem_num = sem_num;
    op.sem_op = -1;
    op.sem_flg = SEM_UNDO;
    return semop(*sem_id, &op, 1);
}

int semCleanUp(int* sem_id){
    if (sem_id > 0) return semctl(*sem_id, 0, IPC_RMID);
    return -1;
}