#!/usr/bin/python
# -*- coding: UTF-8 -*-

import json
from pprint import pprint
import sys

maxTime=150
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
		if maxTime < el['time']:
			nMa=nMa+1



pprint(nMa)


