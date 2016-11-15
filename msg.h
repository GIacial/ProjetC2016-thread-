#ifndef INCLUDED_MSG_H
#define INCLUDED_MSG_H

	#define DROIT 0700


	typedef struct msgbuf
	{
		long mtype;
		char mtext[3];
		
	} msgbuf;

#endif
