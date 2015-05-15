
import socket

import argparse

import json



parser=argparse.ArgumentParser(description="IPv4 address and port")
parser.add_argument('address')
parser.add_argument('port')
args=parser.parse_args()
address=args.address
port=int(args.port)

receivingSocket = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
print "Socket primed."

result = socket.getaddrinfo(address, port, socket.AF_INET, socket.SOCK_DGRAM, 0, 0)


family, socketType, proto, canonName, socketAddress = result[0]
receivingSocket.bind(socketAddress)
print "Socket created."
while 1:
    data,address = receivingSocket.recvfrom(1000)
    print str(data)
    jsonObject = json.loads(str(data))
    print jsonObject['testIdentifier']
    print address
