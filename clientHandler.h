#ifndef BS_10_2024_SUB_H
#define BS_10_2024_SUB_H

#include <stdbool.h>

#include "keyValStore.h"
#include "sub.h"
#include "cmdType.h"

void handleClient(KeyVal* keyValStore, const int cfd, int* sem_id, bool* transaction, SubPool* pool);
void handleConnection(KeyVal* keyValStore, const int cfd, int* sem_id, bool* transaction, SubPool* pool, int msid);
void handleMsgReceive(int cfd, SubPool* pool, int msid);

void signalHandler(int sig);

void handlePut(KeyVal* data, SubPool* pool, int msid, char *response, int* sem_id, bool* transaction, char* in_r);
void handleGet(KeyVal* data, char *response, int* sem_id, bool* transaction, char* in_r);
void handleDel(KeyVal* data, SubPool* pool, int msid, char *response, int* sem_id, bool* transaction, char* in_r);
void handleBeg(char *response, int* sem_id, bool* transaction);
void handleEnd(char *response, int* sem_id, bool* transaction);
void handleSub(KeyVal* keyValStore, SubPool* pool, int msid, char* response, char* in_r);

void sendSubMessage(SubPool* pool, int msid, CmdType cmdType, char* key, char* value);
int sendMessage(int cfd, char* message);
void trimMessage(char *in);

void cleanUp(int signum);

#endif //BS_10_2024_SUB_H
