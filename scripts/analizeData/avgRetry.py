#!/usr/bin/python
# -*- coding: UTF-8 -*-

import json
import sys

tipo=-1
nPkt=0
numRetry=0
try:
	inputFile=sys.argv[1]
	tipo=int(sys.argv[2])
except:
	sys.exit("missing argument")

with open(inputFile) as data_file:    
    data = json.load(data_file)

for el in data["pacchetti"]:
	if el['type']==tipo:
		if el['ack']== True :
			numRetry=numRetry+el['retrycount']
			nPkt=nPkt+1	
	
avg=float(numRetry)/float(nPkt)
print(avg, numRetry, nPkt)

