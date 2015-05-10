CFLAGS=-Wall -o
CC=clang

all:	libsend.c sendrecvUDP.c sendclientapp.c
	$(CC) $(CFLAGS) sendclientapp libsend.c sendrecvUDP.c sendclientapp.c
	

clean: 
	@rm -rfv *.o *.so *.so*

