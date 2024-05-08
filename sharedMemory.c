/*******************************************************************
 * race.c zeigt die Verwendung von Shared Memory.                  *
 * Es werden mehrere Kindprozesse erzeugt, die alle bis zu einer   *
 * bestimmten Zahl MAXCOUNT hochzählen. Dabei lesen und schreiben  *
 * alle Kindprozesse den aktuellen Zählerstand in ein Shared       *
 * Segment. Davon unabhängig zählt jeder Kindprozess in einer      *
 * lokalen Variablen die Anzahl der Schleifendurchläufe.           *
 *******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#define MAXCOUNT 1000000
#define NUM_OF_CHILDS 4
#define SEGSIZE sizeof(int)

int test() {
  int i, id, *shar_mem;   /*  id für das Shared Memory Segment        */
                          /*  mit *shar_mem kann der im Shared Memory */
                          /*  gespeicherte Wert verändert werden      */
  int pid[NUM_OF_CHILDS]; /*  enthält die PIDs der Kindprozesse       */

  /* Nun wird das Shared Memory Segment angefordert, an den Prozess   */
  /* angehängt, und der dort gespreicherte Wert auf 0 gesetzt         */
  id = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT|0600);
  shar_mem = (int *)shmat(id, 0, 0);
  *shar_mem = 0;

  /* Der Vaterprozess erzeugt eine bestimmte Anzahl Kindprozesse      */
  for (i = 0; i < NUM_OF_CHILDS; i++) {
    pid[i] = fork();
    if (pid[i] == -1) {
     printf("Kindprozess konnte nicht erzeugt werden!\n");
     exit(1);
    }
    /* Wenn hier ein Kindprozess arbeitet, wird die Variable count auf 0  */
    /* gesetzt. Dabei ist diese Variable unabhängig von den anderen       */
    /* Prozessen. Anschließend wird in einer while-Schleife geprüft,      */
    /* ob MAXCOUNT schon erreicht wurde. Wenn nicht, wird sowohl der      */
    /* Inhalt des Shared Memory als auch count erhöht.                    */
    /* Nach Erreichen gibt jeder Kindprozeß aus, mit wieviel Durchläufen  */
    /* er zum Erreichen von MAXCOUNT beigetragen hat.                     */

    if (pid[i] == 0) { /* Kindprozess-spezifischer Code */
      int count = 0;
      while (*shar_mem < MAXCOUNT) {
        *shar_mem += 1;
        count++;
      }

      printf("%i Durchläufe wurden benötigt von Kind %d in Prozess %d\n", count, i, getpid());
      exit(0);
     }
   }

   /* Der Vaterprozess wartet, bis alle Kindprozesse fertig sind.  */
   for (i = 0; i < NUM_OF_CHILDS; i++) {
     waitpid(pid[i], NULL, 0);
   }
   printf("Alle %d Kindprozesse wurden beendet.\n", NUM_OF_CHILDS);


   /* Das Shared Memory Segment wird abgekoppelt und freigegeben. */
   shmdt(shar_mem);
   shmctl(id, IPC_RMID, 0);

}