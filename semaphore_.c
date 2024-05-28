/**********************************************************************
 * race_no.c ist eine Weiterentwicklung von race.c                    *
 * Mehrere Kindprozesse greifen auf ein gemeinsames Segment           *
 * zu. Das Programm zeigt die Verwendung von Semaphore.               *
 * Dabei wird das in race.c auftretende gegenseitige Überschreiben    *
 * des Speichers ebenso unterbunden wie das Vermischen der Ausgabe.   *
 **********************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>      // Headerfile für Semaphore
#include <sys/wait.h>
#define MAXCOUNT 1000000
#define NUM_OF_CHILDS 4
#define SEGSIZE sizeof(int)
#define true 1

int test() {
    // Variable sem_id für die Semaphorgruppe und
    // aus technischen Gründen eine Variable marker[1].
    int i, shm_id, sem_id, *shar_mem, pid[NUM_OF_CHILDS];
    unsigned short marker[1];

    // Es folgt das Anlegen der Semaphorgruppe. Es wird hier nur ein
    // Semaphor erzeugt

    sem_id = semget (IPC_PRIVATE, 1, IPC_CREAT|0644);
    if (sem_id == -1) {
        perror ("Die Gruppe konnte nicht angelegt werden!");
        exit(1);
    }

    // Anschließend wird der Semaphor auf 1 gesetzt
    marker[0] = 1;
    semctl(sem_id, 1, SETALL, marker);  // alle Semaphore auf 1

    // Anforderung des Shared Memory Segments
    shm_id = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT|0644);

    if (shm_id == -1) {
        perror ("Das Segment konnte nicht angelegt werden!");
        exit(1);
    }

    shar_mem = (int *)shmat(shm_id, 0, 0);
    *shar_mem = 0;

    // Der Vaterprozess erzeugt eine bestimmte Anzahl Kindprozesse.
    for (i = 0; i < NUM_OF_CHILDS; i++) {

        pid[i] = fork();

        if (pid[i] == -1){
            perror ("Kind konnte nicht erzeugt werden!");
            exit(1);
        }

        if (pid[i] == 0) { // i-tes Kind
            int count = 0;

            struct sembuf enter, leave; // Structs für den Semaphor
            enter.sem_num = leave.sem_num = 0;  // Semaphor 0 in der Gruppe
            enter.sem_flg = leave.sem_flg = SEM_UNDO;
            enter.sem_op = -1; // blockieren, DOWN-Operation
            leave.sem_op = 1;   // freigeben, UP-Operation

            while (true) {
                semop(sem_id, &enter, 1); // Eintritt in kritischen Bereich
                if (*shar_mem < MAXCOUNT) {
                    *shar_mem += 1;
                    semop(sem_id, &leave, 1); // Verlassen des kritischen Bereichs
                    count++;
                } else {
                    semop(sem_id, &leave, 1); // Verlassen des kritischen Bereichs
                    break;
                }
            }

            printf("%i Durchläufe wurden benötigt von Kind %d in Prozess %d.\n", count, i, getpid());

            exit(0);
        }
    }

    // Der Vaterprozess wartet, bis alle Kindprozesse beendet sind.
    for (i = 0; i < NUM_OF_CHILDS; i++) {
        waitpid(pid[i], NULL, 0);
    }

    // Die Semaphorgruppe und das Shared Memory Segment
    // werden wieder freigegeben.

    shmdt(shar_mem);
    shmctl(shm_id, IPC_RMID, 0);
    semctl(sem_id, 0, IPC_RMID);

    return 0;

  }
