#ifndef KEYVALSTORE_H
#define KEYVALSTORE_H

#define KEY_SIZE 1024
#define VAL_SIZE 1024
#define ARRAY_SIZE 1024

typedef struct {
    char key[KEY_SIZE];
    char value[VAL_SIZE];
} Data;

int put(Data *data, char* key, char* value);
int get(Data *data, char* key, char** res);
int del(Data *data, char* key);

#endif //KEYVALSTORE_H
