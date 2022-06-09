CC=gcc

CCFLAGS=-D_DEFAULT_SOURCE -D_XOPEN_SOURCE -D_BSD_SOURCE -std=c11 -pedantic -Wvla -Wall -Werror

ALL= server client pdr maint

all: $(ALL)

server : server.o utils_v1.o
	$(CC) $(CCFLAGS) -o server server.o utils_v1.o
server.o: server.c server.h virement.h
	$(CC) $(CCFLAGS) -c server.c
maint: maint.c utils_v1.o virement.h
	$(CC) $(CFLAGS) -o maint maint.c utils_v1.o
client : client.o utils_v1.o
	$(CC) $(CCFLAGS) -o client client.o utils_v1.o
client.o: client.c virement.h client.h
	$(CC) $(CCFLAGS) -c client.c
pdr : pdr.o utils_v1.o
	$(CC) $(CCFLAGS) -o pdr pdr.o utils_v1.o
pdr.o : pdr.c
	$(CC) $(CCFLAGS) -c pdr.c
utils_v1.o: utils_v1.c utils_v1.h
	$(CC) $(CCFLAGS) -c utils_v1.c

clean:
	rm -f *.o
	rm -f $(ALL)		