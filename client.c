#include <stdio.h>
#include <string.h>
#include<unistd.h>
#include <stdlib.h>
//include pour msg
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>	//msg
#include <sys/sem.h>   //sem    
#include <fcntl.h> //open


#include "msg.h"

//faire la fonction d'envoie de donnees

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
	
	//debut de communication avec le personnel du serveur qui ma pris en charge
	char sortie[4];
	strcpy(sortie,msg.mtext);
	strcat(sortie,"s");
	
	//doit etre ouvert dans le meme sens que dans le serveur
	int deServeur=open(sortie,O_RDONLY);
	if(deServeur==-1){
		fprintf(stderr, "ECHEC DE OUVERTURE PIPE %s\n",sortie );
	}
	
	
	int versServeur=open(msg.mtext,O_WRONLY);
	if(versServeur==-1){
		fprintf(stderr, "ECHEC DE OUVERTURE PIPE %s\n",msg.mtext );
	}
	
	//on fait les demande de service
	int service=0;
	printf("Service:");
	scanf("%d",&service);
	if(write(versServeur ,&service,sizeof(int))==-1){
		fprintf(stderr, "ECHEC DE LA LECTURE \n" );
	}
	
	//reponse alloc lesser 
	if(read(deServeur,&service,sizeof(int))<0){
		fprintf(stderr, "ECHEC DE LA LECTURE DE LA REPONSE DE L'ALLOC \n" );
	}

	if(service != -1){
		//reponse
		if(read(deServeur,&service,sizeof(int))<0){
			fprintf(stderr, "ECHEC DE LA LECTURE \n" );
		}
		printf("Resultat %d\n",service);		
	}else{
		printf("Le service n'est pas disponible pour l'instant \n");
	}
	

	close(versServeur);
	close(deServeur);
	return 0;
}
