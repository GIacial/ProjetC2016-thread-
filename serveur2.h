#ifndef INCLUDED_SERVEUR2_H
#define INCLUDED_SERVEUR2_H

	typedef struct dataThread{
		int numThread;
		int msgidSortie;	//pour envoyer le nom du pipe au client
	}* dataThread;
	
	void* gestionClient(void* data);
	dataThread initDataThread();

#endif
