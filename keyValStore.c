#include "keyValStore.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/shm.h>

int count = -1;

int initKeyValStore(KeyVal** keyValStore, int* shm_id) {
    *shm_id = shmget(IPC_PRIVATE, (ARRAY_SIZE * sizeof(KeyVal)), IPC_CREAT | 0600);
    if (shm_id < 0) {
        perror("shmget failed");
        return -1;
    }

    void *temp = shmat(*shm_id, NULL, 0);
    if (temp == (void *) -1) {
        perror("shmat failed");
        return -1;
    }

    *keyValStore = (KeyVal *)temp;
    return 0;
}

/*
void shmCleanUp(KeyVal* keyValStore, int shm_id) {
    if (keyValStore != NULL) {
        shmdt(keyValStore);
        keyValStore = NULL;
    }

    if (shm_id > 0) shmctl(shm_id, IPC_RMID, NULL);

    exit(0);
}
*/

int put(KeyVal *keyValStore, char* key, char* value) {
    if (key == NULL || value == NULL) {
        fprintf(stderr, "key or value cannot be NULL!\n");
        return -1;
    }
    for (int i = 0; i < ARRAY_SIZE; i++) {
        if (strncmp(keyValStore[i].key, key, KEY_SIZE) == 0) {
            strncpy(keyValStore[i].value, value, VAL_SIZE);
            keyValStore[i].value[VAL_SIZE - 1] = '\0';
            return 0;
        }
    }

    for (int i = 0; i < ARRAY_SIZE; i++) {
        if (keyValStore[i].key[0] == '\0') {
            strncpy(keyValStore[i].key, key, KEY_SIZE);
            keyValStore[i].key[KEY_SIZE - 1] = '\0';

            strncpy(keyValStore[i].value, value, VAL_SIZE);
            keyValStore[i].value[VAL_SIZE - 1] = '\0';
            return 0;
        }
    }
    fprintf(stderr, "No space left in key-value store.\n");
    return -1;
}

int get(KeyVal *keyValStore, char* key, char** res) {
    if (key == NULL || res == NULL) {
        fprintf(stderr, "Invalid pointers in get operation.\n");
        return -1;
    }
    for (int i = 0; i < ARRAY_SIZE; i++) {
        if (strncmp(keyValStore[i].key, key, KEY_SIZE) == 0) {
            *res = keyValStore[i].value;
            return 0;
        }
    }
    return -1;
}

int del(KeyVal *keyValStore, char* key) {
    if (key == NULL) {
        fprintf(stderr, "Invalid pointer in delete operation.\n");
        return -1;
    }
    for (int i = 0; i < ARRAY_SIZE; i++) {
        if (strncmp(keyValStore[i].key, key, KEY_SIZE) == 0) {
            keyValStore[i].key[0] = '\0';
            keyValStore[i].value[0] = '\0';
            return 0;
        }
    }
    return -1;
}



