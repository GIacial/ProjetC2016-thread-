OPTS= -D_XOPEN_SOURCE -Wall -Wextra -pedantic -Wconversion -std=c99 -pthread
COM=gcc 


$compi: client.o serveur.o configuration.o pile.o lesser.o thread.o pipeLesser.o
	@echo compilation client serveur
	@$(COM) -o client.exe $(OPTS) client.o 
	@$(COM) -o serveur.exe $(OPTS) serveur.o  pile.o lesser.o thread.o pipeLesser.o configuration.o 
		
client.o: client.c  msg.h
	@echo compilation client.o
	@$(COM) -c $(OPTS) client.c
		
serveur.o: serveur.c  msg.h pile.h lesser.h thread.h configuration.h pipeLesser.h
	@echo serveur.o
	@$(COM) -c $(OPTS) serveur.c
		

pile.o : pile.c pile.h
	@echo compilation pile.o
	@$(COM) -c $(OPTS) pile.c
	
lesser.o: lesser.c lesser.h
	@echo compilation lesser.o
	@$(COM) -c $(OPTS) lesser.c
	
thread.o: thread.c thread.h msg.h
	@echo compilation thread.o
	@$(COM) -c $(OPTS) thread.c

configuration.o: configuration.c configuration.h
	@echo compilation configuration.o
	@$(COM) -c $(OPTS) configuration.c
	
	
pipeLesser.o: pipeLesser.c pipeLesser.h
	@echo compilation pipeLesser.o
	@$(COM) -c $(OPTS) pipeLesser.c

