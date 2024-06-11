#ifndef KEYVALSTORE_H
#define KEYVALSTORE_H

#define KEY_SIZE 1024
#define VAL_SIZE 1024
#define MAX_SUBS 1024
#define ARRAY_SIZE 1024

typedef struct {
    char key[KEY_SIZE];
    char value[VAL_SIZE];
} KeyVal;

int initKeyValStore(KeyVal** keyValStore, int* shm_id);
void shmCleanUp(KeyVal* keyValStore, int shm_id);

int put(KeyVal *keyValStore, char* key, char* value);
int get(KeyVal *keyValStore, char* key, char** res);
int del(KeyVal *keyValStore, char* key);

#endif //KEYVALSTORE_H
