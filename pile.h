#ifndef PILE_H_INCLU
#define PILE_H_INCLU

#include <stdbool.h>
#include <pthread.h>

typedef struct pile Pile;	//si on mets une etoile ici mieux pour l'utilisateur

Pile* initPile();
void empiler(Pile* p ,pthread_t  val);
void depiler(Pile* p);
bool pileVide(const Pile* p);
void freePile(Pile** p);
pthread_t  lireTete(const Pile* p);

#endif
