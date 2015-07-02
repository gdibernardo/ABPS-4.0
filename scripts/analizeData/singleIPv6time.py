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

time=0


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
		if el['ipVersion']=="ipv6":
			if el['testId']==1 or el['testId']==spa:
				print(time)
				if ini !=0 :
					avg=float(time)/float(npk)

					ar.append(avg)
					ar.append(npk)
				else:
					ini=1
				time=0
				npk=0	
		
			time=time+el['time']
			npk=npk+1



avg=float(time)/float(npk)

ar.append(avg)
ar.append(npk)
print(ar)





