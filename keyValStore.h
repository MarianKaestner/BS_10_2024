//
// Created by annic on 28.04.2024.
//

#ifndef KEYVALSTORE_H
#define KEYVALSTORE_H

#define ARRAY_SIZE 100

int put(char* key, char* value);
int get(char* key, char** res);
int del(char* key);

#endif //KEYVALSTORE_H
