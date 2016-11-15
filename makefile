OPTS= -D_XOPEN_SOURCE -Wall -Wextra -pedantic -Wconversion -std=c99 -pthread
EXEC= main.exe
COM=gcc 



$(EXEC): client.o serveur.o serveur2.o
	@$(COM) -o client.exe $(OPTS) client.o 
	@$(COM) -o serveur.exe $(OPTS) serveur.o serveur2.o
		
client.o: client.c  msg.h
	@$(COM) -c $(OPTS) client.c
		
serveur.o: serveur.c  msg.h serveur2.h serveur2.o
	@$(COM) -c $(OPTS) serveur.c
		
serveur2.o: serveur2.c serveur2.h
	@$(COM) -c $(OPTS) serveur2.c
