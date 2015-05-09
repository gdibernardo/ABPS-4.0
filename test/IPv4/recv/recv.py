import socket

import argparse

parser=argparse.ArgumentParser(description="ipv adress and port")
parser.add_argument('adress')
parser.add_argument('port')
args=parser.parse_args()
adress=args.adress
port=int(args.port)

s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
print "socket primed"


#host=socket.getaddrinfo("ip6-localhost",6666,socket.AF_INET6,socket.SOCK_DGRAM,0,socket.AI_PASSIVE)
#print host
#family, socktype, proto, canonname, sockaddr = host[0]
s.bind((adress,port))
print "socket created"
#for num in xrange(500):
while 1:
	data,addr=s.recvfrom(1000)
	#print str(data)
	print addr
