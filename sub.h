#ifndef SUB_H
#define SUB_H

#include <stdbool.h>
#include "keyValStore.h"

#define MAX_SUBS 1024

typedef struct {
    char key[KEY_SIZE];
    int msids[MAX_SUBS];
    int subCount;
} Sub;

typedef struct {
    Sub subs[ARRAY_SIZE];
    int keyCount;
} SubPool;

int initSubPool(SubPool** pool, int* shm_id);
int sub(SubPool* pool, char* key, int id);
int unSub(SubPool* pool, char* key, int id);
Sub* findSub(SubPool *pool, const char *key);
bool isSub(SubPool *pool, const char *key, int pid);

#endif //SUB_H
