#include <stdio.h>
#include<unistd.h>
#include <stdlib.h>
//include pour msg
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>

#include "msg.h"
#include "serveur2.h"

int main(){
	printf("Lancement Serveur\n");
//creation msg
	int msgidE= msgget(ftok("eServeur",1875),IPC_CREAT |DROIT);
	int msgidS= msgget(ftok("sServeur",1875),IPC_CREAT |DROIT);
	if(msgidE==-1 || msgidS==-1){
		fprintf(stderr, "ECHEC LORS DE LA CREATION DES MSG\n" );
		exit(0);
	}

	msgbuf msg;
	int memo=0;
	pthread_t thd;
	
	printf("Serveur Pret\n");
	
	while (memo!=2){
	
		if(msgrcv(msgidE,&msg,sizeof(msgbuf)-sizeof(long),0,0)==-1){
			fprintf(stderr, "ECHEC DE RECEPTION\n");
		}
	
		printf("j'ai recu %c\n",msg.mtext[0]);
		
		dataThread d=initDataThread();
		d->numThread=memo;
		d->msgidSortie=msgidS;
		if(pthread_create(&thd,NULL,gestionClient,(void*)d)!=0){
			fprintf(stderr, "ECHEC DE CREATION THREAD\n");
		}
		memo++;
				
	}

	pthread_join(thd,NULL);
	
	printf("Fin Serveur\n");

//destruction msg
	struct msqid_ds buf;
	msgctl(msgidE,IPC_RMID,&buf);
	msgctl(msgidS,IPC_RMID,&buf);
	return 0;
}
