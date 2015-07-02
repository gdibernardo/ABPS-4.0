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
except:
	sys.exit("missing argument")

with open(inputFile) as data_file:    
    data = json.load(data_file)

for el in data["pacchetti"]:
	if el['type']==tipo:
		if el['ack']==False:
			numNack=numNack+1
		else:
			if numNack !=0 :
				ar.append(numNack)
			numNack=0




ar.append(numNack)
print(ar)





