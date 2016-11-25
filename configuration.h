#ifndef INCLUDED_CONFIGURATION_H
#define INCLUDED_CONFIGURATION_H

#include <stdbool.h>

typedef struct configData* configData;

typedef struct tabConfigData{
	int nbLesser;						//le nombre de lesser daemons
	configData* serviceLesser;			//tableau des services de chaque Lesser daemons
	bool* serviceLibre;					//tableau des disponibilites de chaque lesser daemons
	pthread_mutex_t accesTabService;	//le mutex de protection de serviceLibre
	
}* tabConfigData;

configData initConfigData(int nbService);
void freeConfigData(configData* d);
tabConfigData chargementData(); 
int getNbService (configData d);
int getService (configData d, int i);
void setService (configData d, int i, int numService);

tabConfigData initTabConfigData(int nbLesser);
void afficheTabConfigData(tabConfigData tab);
void freeTabConfigData(tabConfigData* tab);



#endif
