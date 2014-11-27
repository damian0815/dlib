#!/usr/bin/env python
import os
import re
import math
import string

files = [f for f in os.listdir(".") if ( re.match(r'd-.*\.pd', f) and not re.match(r'.*-help\.pd', f))]
files.sort()


# make four columns, each 120px wide
numColumns = 8
columnWidth = 140
rowHeight = 25
filesPerColumn = int(math.ceil(len(files)/numColumns))

currentFile = 0
print("#N canvas 20 20 "+str(columnWidth*numColumns+10)+" "+str(rowHeight*filesPerColumn+20))

for column in range(filesPerColumn):
	currentX = 10+column*columnWidth;
	for row in range(filesPerColumn):
		if currentFile >= len(files):
			break
		f = files[currentFile]
		f = string.replace(f, '.pd', ';')
		currentY = row*rowHeight;
		print("#X obj "+str(currentX)+" "+str(currentY)+" "+f+"")
		currentFile+=1






