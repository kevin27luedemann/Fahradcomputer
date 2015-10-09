# -*- coding: utf-8 -*-
"""
Created on Tue Oct  6 18:47:01 2015

@author: kevin
"""

from PIL import Image

#Bild laden
im = Image.open("006.png")
pix = im.load()
x = im.size
assert x[0]==64, "Bild falsche Groese"
#print im.size #Get the width and hight of the image for iterating over
print x

#Ausgabedatei Oeffnen
fout = open('006.h',"w")
fout.write('/*\n')
fout.write('\tHeader fuer Pixelmuster von Glurak\n')
fout.write('*/\n')
fout.write('const uint8_t Glurak[][64] PROGMEM = {\n')
for i in range(0,8):
    fout.write('{')
    for j in range(0,x[1]):
        zahl=0
        for k in range(0,8):
            if pix[j,i*8+k][0]==0:
               zahl+=2**k
               print zahl
        if j!=x[1]-1:
            fout.write('{},'.format(zahl))
        else:
            fout.write('{}'.format(zahl))
    if i!=7:
        fout.write('},\n')
    else:
        fout.write('}\n')
fout.write('};\n')
fout.close
