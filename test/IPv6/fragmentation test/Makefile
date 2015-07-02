CFLAGS=-Wall -D_GNU_SOURCE -o
CC=clang

all:	libsend.c sendrecvUDP.c testlib.c ipv6fragmentation.c
	$(CC) $(CFLAGS) ipv6fragmentationapp libsend.c sendrecvUDP.c testlib.c sendclientapp.c -l json
	

clean: 
	@rm -rfv *.o *.so *.so*

