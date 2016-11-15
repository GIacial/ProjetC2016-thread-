#include <stdio.h>
#include<unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>


#include "serveur2.h"
#include "msg.h"

void gestionClient(dataThread d){
	msgbuf msg;
	msg.mtype=1;
	sprintf(msg.mtext,"%d",d->numThread);
	int pipeE= mkfifo(msg.mtext,S_IRUSR);
	
	
	if(msgsnd(d->msgidSortie,&msg,sizeof(msgbuf)-sizeof(long),0)==-1){
			fprintf(stderr, "ECHEC DE L'ENVOI\n" );
		}
	free(d);
}

dataThread initDataThread(){
	dataThread r=(dataThread) malloc(sizeof(struct dataThread));
	return r;
}
