#ifndef MSGQUEUE_H
#define MSGQUEUE_H
#include "keyValStore.h"
#include "cmdType.h"

typedef struct {
    long mtype;
    CmdType cmdType;
    char key[KEY_SIZE];
    char value[VAL_SIZE];
} Msg;

int creMsgQueue(int* msid);
int msgSend(int msid, CmdType cmdType, char* key, char* value);
int msgReceive(int msid, Msg* msg);

#endif //MSGQUEUE_H
