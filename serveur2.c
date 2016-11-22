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


#include "serveur2.h"
#include "msg.h"

struct configData{
		int nbService;
		int *serv;
	};



//--------------------------------------------------------------------------------

void* gestionClient(void* data){
	dataThread d= (dataThread)data;
	static pthread_mutex_t accesMsgS = PTHREAD_MUTEX_INITIALIZER; //protection de l'acces au msg Sserveur
	
	printf("Thread %d operationnel\n",d->numThread);
	msgbuf msg;
	msg.mtype=1;
	
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
	service++;
	
	//envoi de la reponse
	if(write(versClient ,&service,sizeof(int))==-1){
		fprintf(stderr, "ECHEC DE LA LECTURE \n" );
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

//----------------------------------------------------------------------------------------------

configData initConfigData(int nbService){
	configData r=(configData) malloc ( sizeof(struct configData));
	r->nbService = nbService;
	r->serv = (int*) malloc ((unsigned long)nbService*sizeof(int));
	return r; 
}

//----------------------------------------------------------------------------------------------

int getNbService (configData d){
	return d->nbService;
}

//----------------------------------------------------------------------------------------------

int getService (configData d, int i){
	int r=-1;
	if (i>=0 && i< d->nbService){
		r = d->serv[i];
	}else{
		fprintf(stderr, "TU LIS UNE MAUVAISE CASE !\n" );
	}
	return r;
}

//----------------------------------------------------------------------------------------------

void setService (configData d, int i, int numService){
	if (i>=0 && i< d->nbService){
		 d->serv[i]=numService;
	}else{
		fprintf(stderr, "TU TAPPES A COTE GROS DEBILE !\n" );
	}	
}
//----------------------------------------------------------------------------------------------


	void freeConfigData(configData* d){
		if((*d)->serv!=NULL){
			free((*d)->serv);
		}
		free(*d);
		*d=NULL;

	}
//----------------------------------------------------------------------------------------------

tabConfigData chargementData(){
	tabConfigData tab = NULL;
	int nbLesser;
	FILE* file = fopen("config.txt", "r");
	if(file == NULL){
		fprintf(stderr, "IMPOSSIBLE D'OUVRIR LE FICHIER DE CONFIG\n" );
		exit(0);
	}else{
		if(fscanf(file,"%d\n",&nbLesser)==0){//Lecture nb lesser deamon 
			fprintf(stderr, "ECHEC DE LA LECTURE \n" );
		}
		tab = initTabConfigData( nbLesser);
		
		for(int i = 0; i<nbLesser; i++){
			int nbs=0;
			fscanf(file,"%d ",&nbs);
			tab->serviceLesser[i]=initConfigData(nbs);
			for(int j = 0; j<nbs; j++){
				int tmp;
				fscanf(file,"%d ",&tmp);
				setService(tab->serviceLesser[i], j, tmp);
			}
			
		}
	}
	fclose(file);
	return tab;
}

//------------------------------------------------------------------------------------------------
tabConfigData initTabConfigData(int nbLesser){
	tabConfigData r = (tabConfigData)malloc(sizeof(struct tabConfigData));
	r->nbLesser = nbLesser;
	r->serviceLesser = (configData*) malloc (sizeof(configData)*(unsigned long)nbLesser);
	
	r->serviceLibre = (bool*) malloc(sizeof(bool)*(unsigned long)nbLesser);
	for(int i = 0 ; i<nbLesser ; i++){
		r->serviceLibre[i]=true;
	}
	//r->accesTabService = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_init(&(r->accesTabService),NULL);
	return r;
}
//--------------------------------------------------------------------------------
void freeTabConfigData(tabConfigData* tab){
	tabConfigData tabl=*tab;
	for(int i=0 ; i<tabl->nbLesser ; i++){
		if(tabl->serviceLesser[i]!=NULL){
		  freeConfigData(&(tabl->serviceLesser[i]));
		}
	}
	free(tabl->serviceLesser);
	free(tabl->serviceLibre);
	free(tabl);
	*tab=NULL;
}

//-*---------------------------------------------------------------------------

void afficheTabConfigData(tabConfigData tab){
	printf("%d\n",tab->nbLesser);
	
  for (int i=0 ; i<tab->nbLesser ; i++){
  	printf("%d ",getNbService(tab->serviceLesser[i]) );
  	for (int j=0 ; j<getNbService(tab->serviceLesser[i]) ; j++){
  		printf("%d ",getService(tab->serviceLesser[i],j));
  	}
  	printf("\n");
  }
}

//-*---------------------------------------------------------------------------

pipeLesser initPipeLesser(int nbLesser){
	pipeLesser r= (pipeLesser) malloc(sizeof(struct pipeLesser));
	r->pipesLesser= (int**) malloc((unsigned int)nbLesser*sizeof(int*));
	r->accesPipe= (pthread_mutex_t*) malloc((unsigned int)nbLesser*sizeof(pthread_mutex_t));
	
	for(int i=0 ; i< nbLesser ; i++){
		r->pipesLesser[i]=(int*)malloc(sizeof(int)*4);
		if(pipe(r->pipesLesser[i])==-1){
			fprintf(stderr,"PROBLEMES DE CREATION DU PIPE %d\n",i);
		}
		if(pipe(&(r->pipesLesser[i][2]))==-1){
			fprintf(stderr,"PROBLEMES DE CREATION DU PIPE %d\n",i);
		}
		pthread_mutex_init(&(r->accesPipe[i]),NULL);
	}
	return r;
}

//-*---------------------------------------------------------------------------

void freePipeLesser(pipeLesser* p, int nbLesser){
	pipeLesser pipe= *p;
	
	for(int i=0 ; i< nbLesser ; i++){
		for(int j=0 ; j<4 ; j++){
			close(pipe->pipesLesser[i][j]);
		}
		free(pipe->pipesLesser[i]);
		pipe->pipesLesser[i]=NULL;
		
	}
	
	free(pipe->pipesLesser);
	free(pipe->accesPipe);
	
	free(*p);
	(*p)=NULL;

}




