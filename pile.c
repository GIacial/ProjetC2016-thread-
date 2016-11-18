#include "pile.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct cellule{
	struct cellule* suiv;
	pthread_t val;
} Cellule;

struct pile{
	int taille;
	Cellule* pile;
};

//-------------------------------------------------------------------

Pile* initPile(){
	Pile* r = (Pile*) malloc(sizeof(Pile));
	r->taille=0;
	r->pile=NULL;
	return r;
}

//-------------------------------------------------------------------------------------

void empiler(Pile* p, pthread_t val){
	Cellule* tete = (Cellule*) malloc(sizeof(Cellule));
	if(tete!=NULL){
		tete->val=val;
		tete->suiv=p->pile;
		p->pile=tete;
		p->taille++;
	}
}
//-------------------------------------------------------------------------------------

void depiler(Pile* p){
	Cellule* stock=NULL;
	stock=p->pile;			//memo tete
	p->pile=stock->suiv;	//passse au suivant
	free(stock);			//free tete
	p->taille--;
}

//-------------------------------------------------------------------------------------
bool pileVide(const Pile* p){
	return p->pile==NULL;
}

//-------------------------------------------------------------------------------------

void freePile(Pile** p){
    while(!pileVide(*p)){
  		depiler(*p);
  	}
    free(*p);
    *p=NULL;
}

//--------------------------------------------------------------------------------------

pthread_t lireTete(const Pile* p){
	pthread_t r = 0;
	if(!pileVide(p)){
		r=p->pile->val;
	}
	else{
		printf("Lire une pile vide .....\n");
	}
	return r;
}


