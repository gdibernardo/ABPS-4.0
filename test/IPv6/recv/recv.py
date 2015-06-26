
import sys, os

import socket

import argparse

import json

import time


def prepareForLogging():
    os.lseek(logFileDescriptor, 0, os.SEEK_END)


def logJSONObject(jsonObject):
    currentTime = time.strftime('%c')
    testIdentifier = jsonObject['testIdentifier']
    os.write(logFileDescriptor,"%s ABPS just received packet - test identifier:%d \n" %(currentTime, testIdentifier))


parser = argparse.ArgumentParser(description="IPv6 address and port")
parser.add_argument('address')
parser.add_argument('port')
parser.add_argument('path')
args = parser.parse_args()

address = args.address


port = int(args.port)

logPath = args.path

logFileDescriptor = os.open(logPath, os.O_WRONLY | os.O_CREAT | os.O_APPEND, 777)

receivingSocket = socket.socket(socket.AF_INET6,socket.SOCK_DGRAM)
print "Socket primed."

result = socket.getaddrinfo(address, port, socket.AF_INET6, socket.SOCK_DGRAM, 0, 0)


family, socketType, proto, canonName, socketAddress = result[0]
receivingSocket.bind(socketAddress)
print "Socket created."
while 1:
    data, address = receivingSocket.recvfrom(1100)
    print str(data)
    jsonObject = json.loads(str(data))
    logJSONObject(jsonObject)
    print("Just received packet with test identifier %d and message %s" %(jsonObject['testIdentifier'], jsonObject['messageContent']))
    #print address

os.close(logFileDescriptor)
