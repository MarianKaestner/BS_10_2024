#ifndef BS_10_2024_SUB_H
#define BS_10_2024_SUB_H

#include "keyValStore.h"

void handleClient(Data* data, int cfd);

void handlePut(Data* data, const char* command, char *response);
void handleGet(Data* data, const char* command, char *response);
void handleDel(Data* data, const char* command, char *response);
void handleBeg(const char* command, char *response);
void handleEnd(const char* command, char *response);

void trimMessage(char *in);

#endif //BS_10_2024_SUB_H
