#include <stdio.h>
#include<unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <pthread.h>


#include "serveur2.h"
#include "msg.h"

void* gestionClient(void* data){
	dataThread d= (dataThread)data;
	static pthread_mutex_t accesMsgS=PTHREAD_MUTEX_INITIALIZER; //protection de l'acces au msg Sserveur
	
	printf("Thread %d operationnel\n",d->numThread);
	msgbuf msg;
	msg.mtype=1;
	
	sprintf(msg.mtext,"%d",d->numThread);
	
	//creation pipe nome
	if( mkfifo(msg.mtext,DROIT)==-1){
		fprintf(stderr,"Thread %d PROBLEME DE CREATION PIPE N\n",d->numThread);
	}
	
	//acces msg de sortir du serveur
	pthread_mutex_lock(&accesMsgS);
	if(msgsnd(d->msgidSortie,&msg,sizeof(msgbuf)-sizeof(long),0)==-1){
			fprintf(stderr, "ECHEC DE L'ENVOI\n" );
		}
	pthread_mutex_unlock(&accesMsgS);
	//rendu acces

//fin	
	printf("Thread %d Fin\n",d->numThread);
	
	unlink(msg.mtext);//destruction du pipe
	free(d);//desalloc de la struc car passer sans ref
	return NULL;
}

dataThread initDataThread(){
	dataThread r=(dataThread) malloc(sizeof(struct dataThread));
	return r;
}
