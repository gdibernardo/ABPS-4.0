#!/usr/bin/python
# -*- coding: UTF-8 -*-

import json
from pprint import pprint
import sys

maxRetry=-1
tipo=-1
nMa=0
try:
	inputFile=sys.argv[1]
	tipo=int(sys.argv[2])
except:
	sys.exit("missing argument")

with open(inputFile) as data_file:    
    data = json.load(data_file)

for el in data["pacchetti"]:
	if el['type']==tipo:
		if maxRetry < el['retrycount']:
			maxRetry=el['retrycount']


for el in data["pacchetti"]:
	if el['type']==tipo:
		if maxRetry == el['retrycount']:
			nMa=nMa+1


print(maxRetry,nMa)


