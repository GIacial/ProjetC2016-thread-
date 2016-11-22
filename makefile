OPTS= -D_XOPEN_SOURCE -Wall -Wextra -pedantic -Wconversion -std=c99 -pthread
EXEC= main.exe
COM=gcc 



$(EXEC): client.o serveur.o serveur2.o pile.o lesser.o
	@$(COM) -o client.exe $(OPTS) client.o 
	@$(COM) -o serveur.exe $(OPTS) serveur.o serveur2.o pile.o lesser.o
		
client.o: client.c  msg.h
	@$(COM) -c $(OPTS) client.c
		
serveur.o: serveur.c  msg.h serveur2.h serveur2.o pile.h lesser.h
	@$(COM) -c $(OPTS) serveur.c
		
serveur2.o: serveur2.c serveur2.h
	@$(COM) -c $(OPTS) serveur2.c
	
pile.o : pile.c pile.h
	@$(COM) -c $(OPTS) pile.c
	
lesser.o: lesser.c lesser.h
	@$(COM) -c $(OPTS) lesser.c
