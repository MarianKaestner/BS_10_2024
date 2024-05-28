#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include "keyValStore.h"

int shmInit(Data** seg);
void shmCleanUp();

#endif //SHARED_MEMORY_H
