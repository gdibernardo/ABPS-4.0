#!/usr/bin/python
# -*- coding: UTF-8 -*-

import json
import sys
import time

try:
	inputFile=sys.argv[1]
	tipo=int(sys.argv[2])
except:
	sys.exit("missing argument")

with open(inputFile) as data_file:    
    data = json.load(data_file)

for el in data["pacchetti"]:
	if el['type']==tipo:
		if el['ack'] == False :
			print(el['testId'])



