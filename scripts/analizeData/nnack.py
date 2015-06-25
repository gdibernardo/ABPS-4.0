#!/usr/bin/python
# -*- coding: UTF-8 -*-

import json
from pprint import pprint
import sys

maxTime=15
totTime=0
totNum=0
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
		#if el['ack'] == False :
		#nMa=nMa+1
		totTime=totTime+el["time"]
		totNum=totNum+1

media=totTime/totNum

pprint(media)


