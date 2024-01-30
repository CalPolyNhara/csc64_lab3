# Makefile for CPE464 tcp test code
# written by Hugh Smith - April 2019

CC= gcc
CFLAGS= -g -Wall
LIBS = 

OBJS = networks.o gethostbyname.o pollLib.o safeUtil.o

all:   cclient server

cclient: cclient.c send_and_receive.c $(OBJS)
	$(CC) $(CFLAGS) -o cclient cclient.c send_and_receive.c $(OBJS) $(LIBS)

server: server.c $(OBJS)
	$(CC) $(CFLAGS) -o server server.c send_and_receive.c $(OBJS) $(LIBS)

.c.o:
	gcc -c $(CFLAGS) $< -o $@ $(LIBS)

cleano:
	rm -f *.o

clean:
	rm -f server cclient *.o




