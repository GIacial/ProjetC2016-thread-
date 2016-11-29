#include <stdio.h>
#include <unistd.h>
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
				fprintf(stderr, "Essai Prise \n");
				pthread_mutex_lock(&(d->configLesser->accesTabService));
				fprintf(stderr, "Prise \n");
				if (d->configLesser->serviceLibre[i]){
					d->configLesser->serviceLibre[i] = false;
					myLesser = i;
					j =  nbServiceLesser ; 
					i = d->configLesser->nbLesser-1; 
				
				}
				pthread_mutex_unlock(&(d->configLesser->accesTabService));
				fprintf(stderr, "Unlock \n");
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
	
	//echange de donne
	echangeClientLesser(deClient,versClient,d->pipesLesser[myLesser][1],d->pipesLesser[myLesser][2]);
	
	//rend le lesser
	fprintf(stderr, "Essai Prise \n");
	pthread_mutex_lock(&(d->configLesser->accesTabService));
	fprintf(stderr, "Prise \n");	
	d->configLesser->serviceLibre[myLesser] = true;
	pthread_mutex_unlock(&(d->configLesser->accesTabService));
	fprintf(stderr, "Unlock \n");
	
	
	

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

//----------------------------------------------------------------------------------------------
/*
lesserIn est l'entrée du pipe pour envoyer au lesser
lesserOut est la sortie du pipe du lesser
VersClient est l'entrée du pipe pour envoyer au client
*/
void echangeClientLesser(int deClient,int versClient , int lesserIn , int lesserOut)	{
//recup data
	
	//lecture taille donnees
	unsigned long tailleDonnees =0;
	if(read(deClient,&tailleDonnees,sizeof(unsigned long))==-1){
		fprintf(stderr, "ECHEC DE COMMUNICATION THREAD <- Client \n");
		perror("Probleme:");
	}
	
	//lecture donnees
	void* donnees = malloc (tailleDonnees);
	if(read(deClient,donnees,tailleDonnees)==-1){
		fprintf(stderr, "ECHEC DE COMMUNICATION THREAD <- Client \n");
		perror("Probleme:");
	}
	
	//envoyer taille donnes
	if(write(lesserIn ,&tailleDonnees,sizeof(unsigned long))==-1){
		fprintf(stderr, "ECHEC DE L'ECRITURE \n" );
	}
	
	//envoyer donnees au lesser
	if(write(lesserIn ,donnees,tailleDonnees)==-1){
		fprintf(stderr, "ECHEC DE L'ECRITURE \n" );
	}
	free(donnees);//free
	
	//recup taille solution
	if(read(lesserOut,&tailleDonnees,sizeof(unsigned long))==-1){
		fprintf(stderr, "ECHEC DE COMMUNICATION THREAD <- Lesser \n");
		perror("Probleme:");
	}
	
	//recup solution
	void* solution = malloc (tailleDonnees);
	if(read(lesserOut,&solution,tailleDonnees)==-1){
		fprintf(stderr, "ECHEC DE COMMUNICATION THREAD <- LESSER \n");
		perror("Probleme:");
	}
	
	//envoi la taille de la solution
	if(write(versClient ,&tailleDonnees,sizeof(unsigned long))==-1){
		fprintf(stderr, "ECHEC DE L'ECRITURE \n" );
	}
	
	//envoi de la solution du service
	if(write(versClient ,&donnees,tailleDonnees)==-1){
		fprintf(stderr, "ECHEC DE L'ECRITURE \n" );
	}
	free(solution);
}

