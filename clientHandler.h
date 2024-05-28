#ifndef BS_10_2024_SUB_H
#define BS_10_2024_SUB_H

#include "keyValStore.h"

#include <stdbool.h>

void handleClient(Data* data, int cfd, int* sem_id, bool* transaction);

void handlePut(Data* data, char *response, int* sem_id, bool* transaction);
void handleGet(Data* data, char *response, int* sem_id, bool* transaction);
void handleDel(Data* data, char *response, int* sem_id, bool* transaction);
void handleBeg(char *response, int* sem_id, bool* transaction);
void handleEnd(char *response, int* sem_id, bool* transaction);

void trimMessage(char *in);

void cleanUp(int signum);

#endif //BS_10_2024_SUB_H
