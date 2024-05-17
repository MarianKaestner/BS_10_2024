#include "keyValStore.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int count = -1;
Data *data;

int put(char* key, char* value) {
    if (key == NULL || value == NULL) {
        fprintf(stderr, "key or value cannot be NULL!\n");
        return -1;
    }
    for (int i = 0; i < ARRAY_SIZE; i++) {
        if (strncmp(data[i].key, key, KEY_SIZE) == 0) {
            strncpy(data[i].value, value, VAL_SIZE);
            data[i].value[VAL_SIZE - 1] = '\0';
            return 0;
        }
    }

    for (int i = 0; i < ARRAY_SIZE; i++) {
        if (data[i].key[0] == '\0') {
            strncpy(data[i].key, key, KEY_SIZE);
            data[i].key[KEY_SIZE - 1] = '\0';

            strncpy(data[i].value, value, VAL_SIZE);
            data[i].value[VAL_SIZE - 1] = '\0';
            return 0;
        }
    }
    fprintf(stderr, "No space left in key-value store.\n");
    return -1;
}

int get(char* key, char** res) {
    if (key == NULL || res == NULL) {
        fprintf(stderr, "Invalid pointers in get operation.\n");
        return -1;
    }
    for (int i = 0; i < ARRAY_SIZE; i++) {
        if (strncmp(data[i].key, key, KEY_SIZE) == 0) {
            *res = data[i].value;
            return 0;
        }
    }
    return -1;
}

int del(char* key) {
    if (key == NULL) {
        fprintf(stderr, "Invalid pointer in delete operation.\n");
        return -1;
    }
    for (int i = 0; i < ARRAY_SIZE; i++) {
        if (strncmp(data[i].key, key, KEY_SIZE) == 0) {
            data[i].key[0] = '\0';
            return 0;
        }
    }
    return -1;
}



