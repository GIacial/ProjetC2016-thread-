#ifndef INCLUDED_THREAD_H
#define INCLUDED_THREAD_H

#include "configuration.h"

typedef struct dataThread{
	int numThread;					//le numero du thread
	int msgidSortie;				//La file de message qui sort du serveur
	tabConfigData configLesser;		//La structure de donnee du fichier de configuration
	int** pipesLesser;				//tableau des pipes de communication avec les lesser daemons
}* dataThread;

void* gestionClient(void* data);	//le code des threads
dataThread initDataThread();		//creation de la structure de donnee des threads

#endif
