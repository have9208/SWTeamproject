# Makefile

CC=gcc
SOBJS=server.o
COBJS=client.o clientNetwork.o clientFile.o
SOUT=server
COUT=client
CFLAGS=

# all: server client
all: client

# server: $(SOBJS)
#     $(CC) -o $(SOUT) $(SOBJS) $(CFLAGS)

client: $(COBJS)
	$(CC) -o $(COUT) $(COBJS) $(CFLAGS)

# server.o: server.c server.h
#     $(CC) -c server.c $(CFLAGS)

client.o: client.c client.h clientNetwork.o clientFile.o
	$(CC) -c client.c $(CFLAGS)

clientNetwork.o: clientNetwork.c clientNetwork.h network.h
	$(CC) -c clientNetwork.c $(CFLAGS)

clientFile.o: clientFile.c clientFile.h
	$(CC) -c clientFile.c $(CFLAGS)

clean:
	rm -f $(SOBJS) $(COBJS) $(SOUT) $(COUT)