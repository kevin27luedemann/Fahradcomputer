# -*- coding: utf-8 -*-
"""
Created on Tue Oct  6 18:47:01 2015

@author: kevin
"""
import numpy as np
from PIL import Image
import sys

#uebergabe von Dateinamen
name = sys.argv[1]
praef = "./Bilder/"

#Bild laden
ima = Image.open("{}{}.png".format(praef,name))
im = ima.convert("RGBA")
pix = im.load()
x = im.size
assert x[0]<=64, "Bild falsche Groese"
#print im.size #Get the width and hight of the image for iterating over
#print x

#Ausgabedatei Oeffnen
fout = open('{}{}.h'.format(praef,name),"w")
fout.write('/*\n')
fout.write('\tHeader fuer Pixelmuster von {}\n'.format(name))
fout.write('*/\n')
fout.write('const uint8_t P{}[][64] PROGMEM = '.format(name)+'{\n')
for i in range(0,8):
	fout.write('{')
	for j in range(0,64):
		zahl=0
		for k in range(0,8):
			if i*8+k>x[0] or j>x[1]:
				zahl+=0
				print i,k,j
			elif (pix[j,i*8+k][0]+pix[j,i*8+k][1]+pix[j,i*8+k][2])/3.0<50.0:
				zahl+=2**k
				#print zahl
		if j!=x[1]-1:
			fout.write('{},'.format(zahl))
		else:
			fout.write('{}'.format(zahl))
	if i!=7:
		fout.write('},\n')
	else:
		fout.write('}\n')
fout.write('};\n')
fout.close()
