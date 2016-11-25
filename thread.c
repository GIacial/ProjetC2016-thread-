#include <stdio.h>
#include<unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h> //open
#include <stdbool.h>

#include "thread.h"
#include "msg.h"



void* gestionClient(void* data){
	dataThread d = (dataThread)data;
	static pthread_mutex_t accesMsgS = PTHREAD_MUTEX_INITIALIZER; //protection de l'acces au msg Sserveur
	
	printf("Thread %d operationnel\n",d->numThread);
	msgbuf msg;
	msg.mtype = 1;
	
	sprintf(msg.mtext,"%d",d->numThread);
	
	char sortie[4];
	strcpy(sortie,msg.mtext);
	strcat(sortie,"s");
	
	//creation pipe nome
	if( mkfifo(msg.mtext,DROIT)==-1){
		fprintf(stderr,"Thread %d PROBLEME DE CREATION PIPE N\n",d->numThread);
	}
	
	if( mkfifo(sortie,DROIT)==-1){
		fprintf(stderr,"Thread %d PROBLEME DE CREATION PIPE N\n",d->numThread);
	}
	
	//acces msg de sortir du serveur
	pthread_mutex_lock(&accesMsgS);
	if(msgsnd(d->msgidSortie,&msg,sizeof(msgbuf)-sizeof(long),0)==-1){
			fprintf(stderr, "ECHEC DE L'ENVOI\n" );
		}
	pthread_mutex_unlock(&accesMsgS);
	//rendu acces
	
	//debut communication avec le client
	int versClient=open(sortie,O_WRONLY);
	if(versClient==-1){
		fprintf(stderr, "ECHEC DE OUVERTURE PIPE %s\n",sortie );
	}
	
	int deClient=open(msg.mtext,O_RDONLY);
	if(deClient==-1){
		fprintf(stderr, "ECHEC DE OUVERTURE PIPE %s\n",msg.mtext );
	}
	
	//comunication
	int service;
	if(read(deClient,&service,sizeof(int))<0){
		fprintf(stderr, "ECHEC DE LA LECTURE \n" );
	}
	
	//calcul
	printf("Thread %d : service %d\n",d->numThread,service);
	
	int myLesser = -1;
	for(int i = 0 ; i < d->configLesser->nbLesser ; i++){
		int nbServiceLesser= getNbService( d->configLesser->serviceLesser[i] ) ;
		for(int j = 0 ; j <  nbServiceLesser ; j++){
			int serv = getService( d->configLesser->serviceLesser[i] , j) ;
			if (serv == service){
				pthread_mutex_lock(&(d->configLesser->accesTabService));
				if (d->configLesser->serviceLibre[i]){
					d->configLesser->serviceLibre[i] = false;
					myLesser = i;
					j =  nbServiceLesser ; 
					i = d->configLesser->nbLesser-1; 
				}
			}
		} 
	}
	if (myLesser != -1 ) {
		//alloc lesser
		if(write(d->pipesLesser[myLesser][1],&service,sizeof(int))==-1){
			fprintf(stderr, "ECHEC DE COMMUNICATION THREAD -> LESSER \n");
			perror("Probleme:");
		}
	}
	
	//envoi de reponse trouvé ou	pas
	if(write(versClient ,&myLesser,sizeof(int))==-1){
		fprintf(stderr, "ECHEC DE LA REPONSE SERVICE DISPO \n" );
	}
	
	//envoi de la reponse
	if(write(versClient ,&service,sizeof(int))==-1){
		fprintf(stderr, "ECHEC DE L'ECRITURE \n" );
	}
	

//fin	
	printf("Thread %d Fin\n",d->numThread);
	
	
	close(versClient);
	close(deClient);
	unlink(msg.mtext);//destruction du pipe
	unlink(sortie);
	free(d);//desalloc de la struc car passer sans ref
	return NULL;
}

//----------------------------------------------------------------------------------------------

dataThread initDataThread(){
	dataThread r=(dataThread) malloc(sizeof(struct dataThread));
	r->numThread = -1;
	r->msgidSortie = -1;
	r->configLesser = NULL;
	r->pipesLesser = NULL;
	return r;
}

