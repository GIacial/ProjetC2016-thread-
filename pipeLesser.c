#include <stdio.h>
#include<unistd.h>
#include <stdlib.h>
#include <fcntl.h> //open


#include "pipeLesser.h"


//----------------------------------------------------------------------------------------------

int** initPipeLesser(int nbLesser){


	int ** r= (int**) malloc((unsigned int)nbLesser*sizeof(int*));

	for(int i=0 ; i< nbLesser ; i++){
		r[i]=(int*)malloc(sizeof(int)*4);
		if(pipe(r[i])==-1){
			fprintf(stderr,"PROBLEMES DE CREATION DU PIPE %d\n",i);
		}
		if(pipe(&(r[i][2]))==-1){
			fprintf(stderr,"PROBLEMES DE CREATION DU PIPE %d\n",i);
		}

	}
	return r;

}

//-*---------------------------------------------------------------------------

void freePipeLesser(int*** p, int nbLesser){
	int** pipe= *p;
	int signalFin = -1;
	
	for(int i=0 ; i< nbLesser ; i++){

		if(write(pipe[i][1] ,&signalFin,sizeof(int))==-1){
			fprintf(stderr, "ECHEC DE L'ENVOI DE LA FIN \n" );
		}
		sleep(3);//sleep pour que les lesser lisent la fin
		//a changer TODO
		for(int j=0 ; j<4 ; j++){
			close(pipe[i][j]);
		}
		free(pipe[i]); //second tab
		pipe[i]=NULL;
		
	}
	
	free(pipe); //premier tab

	(*p)=NULL;

}

