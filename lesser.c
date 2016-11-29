#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "service.h"
#include "lesser.h"


void mainLesser(int pipeIn, int pipeOut, int numFils){
	printf("Coucou je suis le fils n%d\n",numFils);
	
	//read  du service 
	int service = -1;
	if(read(pipeIn ,&service,sizeof(int))==-1){
		fprintf(stderr, "ECHEC DE LECTURE SERVICE \n" );
	}
	while (service!=-1){
		//calcul
		switch(service){
			case 1 : Service1(pipeOut ,  pipeIn);
			break;
			
			default:	fprintf(stderr,"Le Service n'est pas fait");
			break;
		}
	

	
	}
}


//------------------------------------------------------------------------------
void Service1(int pipeOut , int pipeIn){
	//recup taille donnees
	unsigned long taille = 0;
	if(read(pipeIn ,&taille,sizeof(unsigned long))==-1){
		fprintf(stderr, "ECHEC DE LECTURE SERVICE \n" );
	}
	
	//recup donnees
	float a,b;
	if(read(pipeIn ,&a,sizeof(float))==-1){
		fprintf(stderr, "ECHEC DE LECTURE \n" );
	}
	if(read(pipeIn ,&b,sizeof(float))==-1){
		fprintf(stderr, "ECHEC DE LECTURE \n" );
	}
	
	//calcul
	float res = addition_Float(a,b);
	
	//envoyer taille res
	taille = (int)sizeof(float);
	if(write(pipeOut ,&taille,sizeof(unsigned long))==-1){
		fprintf(stderr, "ECHEC DE L'ENVOI DE LA TAILLE DE SOLUTION \n" );
	}
	
	//envoyer res
	if(write(pipeOut ,&res,taille)==-1){
		fprintf(stderr, "ECHEC DE L'ENVOI DE LA  SOLUTION \n" );
	}


}
