#!/usr/bin/python
# -*- coding: UTF-8 -*-

import json
import sys
import time

num=0
nLoss=0
try:
	inputFile=sys.argv[1]
	tipo=int(sys.argv[2])
except:
	sys.exit("missing argument")

with open(inputFile) as data_file:    
    data = json.load(data_file)

for el in data["pacchetti"]:
	if el['type']==tipo:
		num=num+1
		if el['ack']==False:
			nLoss=nLoss+1



avg=float(nLoss)/float(num)
avg=avg*100
print(avg)
