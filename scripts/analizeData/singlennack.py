#!/usr/bin/python
# -*- coding: UTF-8 -*-

import json
import sys

tipo=-1
nMa=0
ar=[]

ini=0
nnack=0
npk=0

numNack=0


try:
	inputFile=sys.argv[1]
	tipo=int(sys.argv[2])
	spa=int(sys.argv[3])
except:
	sys.exit("missing argument")

with open(inputFile) as data_file:    
    data = json.load(data_file)

for el in data["pacchetti"]:
	if el['type']==tipo:
		ris=float(el['testId'])/float(spa)
		if el['testId']==1 or (ris % 2 == 0):
			#print(numNack)
			if ini !=0 :			
				ar.append(numNack)
			else:
				ini=1
			numNack=0
		
		if el['ack']==False:
			numNack=numNack+1




ar.append(numNack)
print(ar)





