# Makefile

CC=gcc
SOBJS=server.o serverNetwork.o
COBJS=client.o clientNetwork.o
SOUT=server
COUT=client
CFLAGS=

# all: server client
all: server client

server: $(SOBJS)
	$(CC) -o $(SOUT) $(SOBJS) $(CFLAGS)

client: $(COBJS)
	$(CC) -o $(COUT) $(COBJS) $(CFLAGS)

server.o: server.c server.h serverNetwork.h
	$(CC) -c server.c $(CFLAGS)

client.o: client.c client.h clientNetwork.h
	$(CC) -c client.c $(CFLAGS)

serverNetwork.o: serverNetwork.c serverNetwork.h network.h
	$(CC) -c serverNetwork.c $(CFLAGS)

clientNetwork.o: clientNetwork.c clientNetwork.h network.h
	$(CC) -c clientNetwork.c $(CFLAGS)

clean:
	rm -f $(SOBJS) $(COBJS) $(SOUT) $(COUT)