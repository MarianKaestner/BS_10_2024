#include "keyValStore.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int count = -1;

typedef struct _Data{
    char *key;
    char *value;
} Data;


Data data[ARRAY_SIZE];

int put(char* key, char* value) {
    if (key == NULL || value == NULL) {
        fprintf(stderr, "key or value cannot be NULL!\n");
        return -1;
    }
    char *res;
    const int index = get(key, &res);

    if (index < 0) {
        if (count < ARRAY_SIZE - 1) {
            data[++count].key = strdup(key);
            data[count].value = strdup(value);
            if (data[count].key == NULL || data[count].value == NULL) {
                fprintf(stderr, "memory allocation failed!\n");
                return -1;
            }
        } else {
            fprintf(stderr, "array full!\n");
            return -1;
        }
    } else {
        free(data[index].value);
        data[index].value = strdup(value);
        if (data[index].value == NULL) {
            fprintf(stderr, "memory allocation for new value failed!\n");
            return -1;
        }
    }

    return 0;
}

int get(char* key, char** res) {
    if (key == NULL || res == NULL) {
        fprintf(stderr, "invalid pointer!\n");
        return -1;
    }

    for (int i = 0; i <= count; i++) {
        if (strcmp(data[i].key, key) == 0) {
            *res = data[i].value;
            return i;
        }
    }

    return -1;
}

int del(char* key) {
	char* res = NULL;
    const int index = get(key, &res);

    if (index < 0) {
		return -1;
	}

    free(data[index].key);
    free(data[index].value);

    data[index].key = NULL;
    data[index].value = NULL;

    for (int i = index; i < count; i++) {
        data[i] = data[i + 1];
    }

    count--;
    return 0;
}



