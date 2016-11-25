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

#include "configuration.h"


struct configData{
		int nbService;
		int *serv;
};


//--------------------------------------------------------------------------------

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

