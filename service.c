#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "pile.h"

float addition_Float(float a ,float b){
	return a+b;
}

float addition_Ligne_Matrice(float** matrice, int larg, int numLigne){
	float res = 0.0;	
	fprintf(stderr,"ligne %d\n",numLigne);
	for (int i = 0; i<larg; i++){
		
	fprintf(stderr,"colonne %d\n",i);
		res += matrice[numLigne][i];
		
	}	
	fprintf(stderr,"res ligne %d = %f\n",numLigne , res);
	return res;
}

typedef struct paramDataThread {
	int numThread;
	float** matrice;
	int nbLigne;
	int largeur;
	int hauteur;
	int nbThread;
}* paramDataThread;

void* resThreadMatrice (void* d){
	paramDataThread dataThread = (paramDataThread )d;
	int debut = dataThread->numThread * dataThread->nbLigne;
	int fin = debut + dataThread->nbLigne;
	if(dataThread->numThread == dataThread->nbThread-1){
		fin = dataThread->hauteur;
	}
	float* res = (float*) malloc(sizeof(float));
	(*res) = 0.0;
	fprintf(stderr,"th %d ->cc\n",dataThread->numThread);
	for (int i= debut ; i < fin && i < dataThread->hauteur ; i++){
		(*res) += addition_Ligne_Matrice(dataThread->matrice, dataThread->largeur , i);
		fprintf(stderr,"th %d ->resultat ligne %d = %f\n",dataThread->numThread,i , *res);
	}
	return (void*)res;
}



float addition_Matrice(float** matrice, int larg, int haut){
	
	
	if(haut==0){
		fprintf(stderr,"Matrice inexistante\n");		
		return 0.0;	
	}

	int nbThread = (rand()%haut)+1;
	fprintf(stderr,"nb Thread %d\n",nbThread);
	paramDataThread *tabNumThread = (paramDataThread*)malloc (sizeof(paramDataThread)*(unsigned long)nbThread);

	Pile* pileThread = initPile();

	pthread_t thd;

	for(int i = 0 ; i<nbThread ; i++){
		fprintf(stderr,"Boucle %d\n",i);
		tabNumThread[i] = (paramDataThread) malloc (sizeof(struct paramDataThread));
		tabNumThread[i]->numThread = i;
		tabNumThread[i]->matrice = matrice;
		tabNumThread[i]->nbLigne = haut/nbThread ;
		tabNumThread[i]->largeur = larg;
		tabNumThread[i]->hauteur = haut;
		tabNumThread[i]->nbThread = nbThread;

		if(pthread_create(&thd,NULL,resThreadMatrice,(void*)(tabNumThread[i]))!=0){
			fprintf(stderr, "ECHEC DE CREATION THREAD\n");
		}else{
			empiler(pileThread,thd);
		}
	}

	float res = 0.0;
	float* tempRes = NULL;
	while(!pileVide(pileThread)){
		pthread_join(lireTete(pileThread),(void**)&tempRes);
		res += (*tempRes);
		free(tempRes);
		depiler(pileThread);
	}
	fprintf(stderr,"resultat %f\n",res);
	return res;

}

char* dateHeure(){
	time_t t;
	time(&t);
	return ctime(&t);
}
