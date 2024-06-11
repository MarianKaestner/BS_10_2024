#include "sub.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/shm.h>

#define INITIAL_CAPACITY 10

int initSubPool(SubPool** pool, int* shm_id) {
	*shm_id = shmget(IPC_PRIVATE, sizeof(SubPool), IPC_CREAT | 0600);
    if (shm_id < 0) {
        perror("shmget failed");
        return -1;
    }

    void *temp = shmat(*shm_id, NULL, 0);
    if (temp == (void *) -1) {
        perror("shmat failed");
        return -1;
    }

    *pool = (SubPool*)temp;

    (*pool)->keyCount = 0;
    return 0;
}

int sub(SubPool* pool, char* key, int id) {
    printf("Attempting to subscribe ID %d to key '%s'.\n", id, key);

    Sub* sub = findSub(pool, key);
    if (sub == NULL) {
        printf("Creating new subscription for key '%s'.\n", key);
        if (pool->keyCount >= MAX_SUBS) {
            perror("SubPool full, cannot add new subscription");
            return -1;
        }
        sub = &pool->subs[pool->keyCount++];
        strncpy(sub->key, key, KEY_SIZE);
        sub->key[KEY_SIZE - 1] = '\0';
        sub->subCount = 0;
    }

    if (sub->subCount >= MAX_SUBS) {
        perror("Max capacity reached for subscription");
        return -1;
    }

    printf("Adding ID %d to subscription for key '%s'.\n", id, key);
    sub->msids[sub->subCount++] = id;
    printf("Successfully subscribed ID %d to key '%s'.\n", id, key);

    return 0;
}

int unSub(SubPool* pool, char* key, int id) {
    Sub *sub = findSub(pool, key);
    if (sub == NULL) {
        fprintf(stderr, "Subscription for key '%s' not found.\n", key);
        return -1;
    }

    int found = 0;
    for (int i = 0; i < sub->subCount; i++) {
        if (sub->msids[i] == id) {
            printf("Removing ID %d from the subscription for key '%s'.\n", id, key);
            sub->msids[i] = sub->msids[--sub->subCount];
            found = 1;
            break;
        }
    }

    if (!found) {
        fprintf(stderr, "Subscriber ID %d for key '%s' not found.\n", id, key);
        return -1;
    }

    printf("Successfully unsubscribed ID %d from key '%s'. Current subscriber count: %d.\n", id, key, sub->subCount);

    return 0;
}

Sub* findSub(SubPool *pool, const char *key) {
    if (pool == NULL) {
        printf("findSub: Warnung - Der übergebene SubPool ist NULL.\n");
        return NULL;
    }

    if (key == NULL) {
        printf("findSub: Warnung - Der übergebene Schlüssel ist NULL.\n");
        return NULL;
    }

    printf("findSub: Suche nach Abonnement für den Schlüssel '%s'.\n", key);

    for (int i = 0; i < pool->keyCount; i++) {
        if (!pool->subs[i].key) {
            printf("findSub: Info - Kein Schlüssel zugewiesen für das Abonnement an der Position %d.\n", i);
            continue;
        }

        if (strcmp(pool->subs[i].key, key) == 0) {
            printf("findSub: Erfolg - Abonnement für den Schlüssel '%s' an der Position %d gefunden.\n", key, i);
            return &(pool->subs[i]);
        }
    }

    printf("findSub: Kein Abonnement für den Schlüssel '%s' gefunden.\n", key);
    return NULL;
}

bool isSub(SubPool *pool, const char *key, int pid) {
    Sub* sub = findSub(pool, key);
    printf("key: [%s], pid [%i]\n", key, pid);
    if(sub != NULL) {
        for(int i = 0; i < sub->subCount; i++) {
            printf("pid [%i]\n", sub->msids[i]);
            if(sub->msids[i] == pid) {
                return true;
            }
        }
    }

    return false;
}