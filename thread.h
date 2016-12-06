#ifndef INCLUDED_THREAD_H
#define INCLUDED_THREAD_H


#include <stdbool.h>
#include "configuration.h"

typedef struct dataThread{
	int numThread;					//le numero du thread
	int msgidSortie;				//La file de message qui sort du serveur
	tabConfigData configLesser;		//La structure de donnee du fichier de configuration
	int** pipesLesser;				//tableau des pipes de communication avec les lesser daemons
}* dataThread;

void* gestionClient(void* data);	//le code des threads
dataThread initDataThread();		//creation de la structure de donnee des threads

void receptionClient(int deClient,int versClient , int lesserIn , int lesserOut, int numThread);	//echange les donne entre client et le thread puis la communication normal
void echangeClientLesser(int versClient , int lesserOut, int numThread);	//echange la solution entre le thread et le lesser puis il envoie la reponse au client
int  threadChoixLesser(dataThread d, int versClient, int service);

#endif

