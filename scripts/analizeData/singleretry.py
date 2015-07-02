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

numRetry=0


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
		if el['testId']==1 or el['testId']==spa:
			print(el['testId'])
			if ini !=0 :
				avg=float(numRetry)/float(npk)
				print (npk)
				ar.append(avg)
				ar.append(npk)
			else:
				print("dentro else")
				ini=1

			numRetry=0
			npk=0	
		
		numRetry=numRetry+el['retrycount']
		npk=npk+1



avg=float(numRetry)/float(npk)

ar.append(avg)
ar.append(npk)
print(npk)
print(ar)





