#ifndef INCLUDED_SERVEUR2_H
#define INCLUDED_SERVEUR2_H

	typedef struct dataThread{
		int numThread;
		int msgidSortie;	//pour envoyer le nom du pipe au client
	}* dataThread;
	
	typedef struct configData* configData;
	
	typedef struct tabConfigData{
		int nbLesser;
		configData* serviceLesser;
	}* tabConfigData;
	
	void* gestionClient(void* data);
	dataThread initDataThread();
	
	configData initConfigData(int nbService);
	void freeConfigData(configData* d);
	tabConfigData chargementData(); 
	int getNbService (configData d);
	int getService (configData d, int i);
	void setService (configData d, int i, int numService);
	
	tabConfigData initTabConfigData(int nbLesser);
	void freeTabConfigData(tabConfigData* tab);

#endif
