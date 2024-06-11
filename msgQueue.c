#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>

#include "cmdType.h"
#include "msgQueue.h"

#include <errno.h>
#include <string.h>

int creMsgQueue(int* msid){
	*msid= msgget(IPC_PRIVATE, IPC_CREAT|0666);
	if(*msid == -1) {
		printf("cannot get message queue\n");
		return -1;
	}

	return 0;
}

int msgSend(int msid, CmdType cmdType, char* key, char* value) {
	Msg msg;
	msg.mtype = 1;
	msg.cmdType = cmdType;
	strncpy(msg.key, key, KEY_SIZE - 1);
	msg.key[KEY_SIZE - 1] = '\0';
	strncpy(msg.value, value, VAL_SIZE - 1);
	msg.value[VAL_SIZE - 1] = '\0';

	printf("Versuche, Nachricht zu senden: CmdType: %d, Key: '%s', Value: '%s'\n", cmdType, key ? key : "NULL", value ? value : "NULL");

	int rc = msgsnd(msid, &msg, sizeof(msg) - sizeof(long), 0);

	if(rc < 0) {
		perror("Fehler beim Schreiben in die Nachrichtenwarteschlange");
		printf("Detaillierter Fehler: %s\n", strerror(errno)); // Bietet detailliertere Fehlerinformationen
		return -1;
	}

	printf("Nachricht erfolgreich gesendet zur Message Queue [msid: %d]\n", msid);
	return 0;
}

int msgReceive(int msid, Msg* msg) {
	printf("Empfange Nachricht aus der Message Queue [msid: %d]\n", msid);

	int rc = msgrcv(msid, msg, sizeof(Msg) - sizeof(long), 0, 0);

	if(rc < 0) {
		perror("Fehler beim Lesen aus der Nachrichtenwarteschlange");
		printf("Detaillierter Fehler: %s\n", strerror(errno)); // Bietet detailliertere Fehlerinformationen
		return -1;
	}

	// Geht davon aus, dass CmdType, Key und Value korrekt gesetzt sind; ggf. Anpassung erforderlich
	printf("Empfangene Nachricht: CmdType: %d, Key: '%s', Value: '%s'\n", msg->cmdType, msg->key ? msg->key : "NULL", msg->value ? msg->value : "NULL");

	return 0;
}