#ifndef SEMAPHORE_H
#define SEMAPHORE_H

int semInit(int* sem_id, int count);
int semUp(int* sem_id, int sem_num);
int semDown(int* sem_id, int sem_num);
int semCleanUp(int* sem_id);

#endif //SEMAPHORE_H
