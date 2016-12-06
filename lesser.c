#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "service.h"
#include "lesser.h"


void mainLesser(int pipeIn, int pipeOut, int numFils){
	printf("Coucou je suis le fils n%d\n",numFils);
	
	//read  du service 
	int service = -2;
	if(read(pipeIn ,&service,sizeof(int))==-1){
		fprintf(stderr, "ECHEC DE LECTURE SERVICE \n" );
	}
	while (service!=-1){
	
		//calcul
		switch(service){
			case 1 : Service1(pipeOut ,  pipeIn);
			break;
			
			case 2 : Service2(pipeOut ,  pipeIn);
			break;
			
			default:	fprintf(stderr,"Le Service n'est pas fait\n");
			break;
		}
	
		if(read(pipeIn ,&service,sizeof(int))==-1){
			fprintf(stderr, "ECHEC DE LECTURE SERVICE \n" );
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
	fprintf(stderr,"Sercive 1 res %f+ %f =%f\n",a,b,res);
	
	//envoyer taille res
	taille = (unsigned long)sizeof(float);
	if(write(pipeOut ,&taille,sizeof(unsigned long))==-1){
		fprintf(stderr, "ECHEC DE L'ENVOI DE LA TAILLE DE SOLUTION \n" );
	}
	
	//envoyer res
	if(write(pipeOut ,&res,taille)==-1){
		fprintf(stderr, "ECHEC DE L'ENVOI DE LA  SOLUTION \n" );
	}
}

//----------------------------------------------------------------------------------
void Service2(int pipeOut, int pipeIn){
	//recup taille donnees
	unsigned long taille = 0;
	if(read(pipeIn ,&taille,sizeof(unsigned long))==-1){
		fprintf(stderr, "ECHEC DE LECTURE SERVICE \n" );
	}
	
	//recup donnees
	float ** matrice =NULL;
	int hauteur , largeur;
	if(read(pipeIn ,&largeur,sizeof(int))==-1){
		fprintf(stderr, "ECHEC DE LECTURE \n" );
	}
	if(read(pipeIn ,&hauteur,sizeof(int))==-1){
		fprintf(stderr, "ECHEC DE LECTURE \n" );
	}
	
	matrice = (float**)malloc (sizeof(float*)* (unsigned long)hauteur);
	for(int i=0 ; i<hauteur ; i++){
		matrice[i]=malloc (sizeof(float)* (unsigned long)largeur);
		if(read(pipeIn ,matrice[i],sizeof(float)* (unsigned long)largeur)==-1){
			fprintf(stderr, "ECHEC DE LECTURE \n" );
		}
	}
	
	for (int i= 0 ; i<hauteur ; i++){
		for(int j=0 ; j<largeur ; j++){
			fprintf(stderr, "M[%d][%d] = %f \n",i,j,matrice[i][j] );
		}
	}
	//calcul
	float res = addition_Matrice(matrice , largeur , hauteur);
	fprintf(stderr,"res  %f\n", res);
	
	//envoyer taille res
	taille = (unsigned long)sizeof(float);
	if(write(pipeOut ,&taille,sizeof(unsigned long))==-1){
		fprintf(stderr, "ECHEC DE L'ENVOI DE LA TAILLE DE SOLUTION \n" );
	}
	
	//envoyer res
	if(write(pipeOut ,&res,taille)==-1){
		fprintf(stderr, "ECHEC DE L'ENVOI DE LA  SOLUTION \n" );
	}
	
	//free matrice
	for(int i =0 ; i < hauteur ; i++){
		free(matrice[i]);
		matrice[i] = NULL;
	}
	free(matrice);
}
