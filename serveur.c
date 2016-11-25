#include <stdio.h>
#include<unistd.h>
#include <stdlib.h>
//include pour msg
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <sys/sem.h>

#include "msg.h"
#include "configuration.h"
#include "thread.h"
#include "pipeLesser.h"
#include "pile.h"
#include "lesser.h"

int main(){
	
	//creation msg
	int msgidE= msgget(ftok("eServeur",1875),IPC_CREAT |DROIT);
	int msgidS= msgget(ftok("sServeur",1875),IPC_CREAT |DROIT);
	if(msgidE==-1 || msgidS==-1){
		fprintf(stderr, "ECHEC LORS DE LA CREATION DES MSG\n" );
		exit(0);
	}
	
	printf("Lancement Serveur\n");
	
	// lecture du fichier de config	
	tabConfigData serviceLesser=chargementData();
	int** pipeAnonymes= initPipeLesser(serviceLesser->nbLesser);
	
	
	// afficheTabConfigData(serviceLesser);
	
	// creation des lesser demons
	for (int i=0 ; i< serviceLesser->nbLesser ; i++){
		if(fork()==0){//si fils
		mainLesser(pipeAnonymes[i][0],pipeAnonymes[i][3],i);
			exit(0);//fin fils
		}
	}

	
	
	//creation du semaphore qui protege
	int sidAccesServeur=semget(ftok("AccesServeur",1875),1,IPC_CREAT|DROIT);
	if(sidAccesServeur==-1){
		fprintf(stderr, "ECHEC LORS DE LA CREATION DE LA PROTECTION DES FILES DE MESSAGES\n" );
		perror("Probleme:");
		exit(0);
	}
	//init de la valeur du sema
	semctl(sidAccesServeur,0,SETVAL,1);
	
	

	msgbuf msg;
	int memo=0;
	pthread_t thd;
	Pile* pileThread = initPile();
	
	printf("Serveur Pret\n");
	
	while (memo!=2){
	
		if(msgrcv(msgidE,&msg,sizeof(msgbuf)-sizeof(long),0,0)==-1){
			fprintf(stderr, "ECHEC DE RECEPTION\n");
		}
	
		printf("j'ai recu %c\n",msg.mtext[0]);
		
		dataThread d=initDataThread();
		d->numThread=memo;
		d->msgidSortie=msgidS;
		d->configLesser = serviceLesser;
		d->pipesLesser =pipeAnonymes;
		if(pthread_create(&thd,NULL,gestionClient,(void*)d)!=0){
			fprintf(stderr, "ECHEC DE CREATION THREAD\n");
		}else{
			empiler(pileThread,thd);
		}
		memo++;
				
	}

	
	while(!pileVide(pileThread)){
		pthread_join(lireTete(pileThread),NULL);
		depiler(pileThread);
	}
	printf("Fin Serveur\n");

	//destruction des files de messages
	freePile(&pileThread);
	msgctl(msgidE,IPC_RMID,NULL);
	msgctl(msgidS,IPC_RMID,NULL);

	//destruction du semaphore
	semctl(sidAccesServeur,0,IPC_RMID,NULL);//0 pour le premier semaphore
	
	// desallocation des structures de donnees
	freePipeLesser(&pipeAnonymes,serviceLesser->nbLesser);
	freeTabConfigData(&serviceLesser);//free serviceLesser
	return 0;
}
