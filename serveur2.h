#ifndef INCLUDED_SERVEUR2_H
#define INCLUDED_SERVEUR2_H

	#include <stdbool.h>
	

	
	typedef struct configData* configData;
	
	typedef struct tabConfigData{
		int nbLesser;
		configData* serviceLesser;
		bool* serviceLibre;
		pthread_mutex_t accesTabService;
		
	}* tabConfigData;
	
	typedef struct pipeLesser{
		int** pipesLesser;
		pthread_mutex_t* accesPipe;
	}* pipeLesser;
	
	typedef struct dataThread{
		int numThread;
		int msgidSortie;	//pour envoyer le nom du pipe au client
		tabConfigData configLesser;
		pipeLesser pipesLesser;
	}* dataThread;
	
	void* gestionClient(void* data);
	dataThread initDataThread();
	
	
	configData initConfigData(int nbService);
	void freeConfigData(configData* d);
	tabConfigData chargementData(); 
	int getNbService (configData d);
	int getService (configData d, int i);
	void setService (configData d, int i, int numService);
	
	tabConfigData initTabConfigData(int nbLesser);
	void afficheTabConfigData(tabConfigData tab);
	void freeTabConfigData(tabConfigData* tab);
	
	pipeLesser initPipeLesser(int nbLesser);
	void freePipeLesser(pipeLesser* p,int nbLesser);

#endif
