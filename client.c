#include <stdio.h>
#include <string.h>
#include<unistd.h>
#include <stdlib.h>
//include pour msg
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>

#include "msg.h"

int main(){

//creation msg
	int msgidE= msgget(ftok("eServeur",1875),IPC_CREAT |DROIT);
	int msgidS= msgget(ftok("sServeur",1875),IPC_CREAT|DROIT);
	if(msgidE==-1 || msgidS==-1){
		fprintf(stderr, "ECHEC LORS DE LA CREATION DES MSG\n" );
		exit(0);
	}
	
	int sidAccesServeur=semget(ftok("AccesServeur",1875),1,IPC_CREAT|DROIT);
	if(sidAccesServeur==-1){
		fprintf(stderr, "ECHEC LORS DE LA CREATION DE LA PROTECTION DES FILES DE MESSAGES\n" );
		perror("Probleme:");
		exit(0);
	}
	struct sembuf lock={0,-1,0};
	struct sembuf unlock={0,1,0};

	msgbuf msg={1,{'f','c','\n'}};
	
	//debut client
	//demande de connexion
	if(semop(sidAccesServeur,&lock,1)==-1){
		fprintf(stderr, "ECHEC LORS DE LA PRISE D'ACCES\n" );
	}
	
	printf("Acces au serveur\n");
	
		if(msgsnd(msgidE,&msg,sizeof(msgbuf)-sizeof(long),0)==-1){
			fprintf(stderr, "ECHEC DE L'ENVOI\n" );
		}

		if(msgrcv(msgidS,&msg,sizeof(msgbuf)-sizeof(long),0,0)==-1){
			fprintf(stderr, "ECHEC DE RECEPTION\n");
		}
		
	printf("Fin d'acces au serveur\n");
		
	//on a recu la confirmation	
	if(semop(sidAccesServeur,&unlock,1)==-1){
		fprintf(stderr, "ECHEC LORS DE LA PRISE D'ACCES\n" );
	}

	printf("j'ai recu %c\n",msg.mtext[0]);
	


	return 0;
}
