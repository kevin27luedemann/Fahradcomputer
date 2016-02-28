# -*- coding: utf-8 -*-
import numpy as np
import numpy.random as rnd
import scipy as sp
import matplotlib as mt
import scipy.integrate as inte
#mt.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
from pylab import *
import sys
import time as ti

name = sys.argv[1]
print name

def latitu(x):
	return (x-x[0])*(4e4/360.0)

def longit(x,y):
	return latitu(y)*np.cos(np.pi*(x-x[0])/180.0)

def getdata(name):
	daten = np.array(np.genfromtxt("{}.TXT".format(name), comments='#', delimiter='\t'))
	daten[:,1] /= 1e6
	daten[:,2] /= 1e5
	daten[:,3] /= 100.0
	daten[:,4] /= 100.0
	daten[:,5] /= 10.0
#	daten[:,6] /= 1.0
	daten[:,7] /= 10.0
	daten[:,0] -= daten[0,0]
	return daten

def getgpxfile(name):
	aus = open("{}.gpx".format(name), "w")
	daten = getdata(name)
	#erstellen des Header
	aus.write('<?xml version="1.0" ?>\n')
	aus.write('<gpx \n version="1.0" \n creator="Kevin Luedemann">\n')
	aus.write('\t<trk>\n')
	aus.write("\t\t<name>{}</name>\n".format(name))
	aus.write('\t\t<trkseg>\n')
	
	#bearbeiten der Punkte in Trackpints fuer das gpx
	for i in range(len(daten[:,0])):
		aus.write("\t\t\t<trkpt lat=\"{}\" lon=\"{}\"> <ele>{}</ele> </trkpt>\n".format(daten[i,2],daten[i,1],daten[i,7]))

	
	
	#ende der Bereiche
	aus.write('\t\t</trkseg>\n')
	aus.write('\t</trk>\n')
	aus.write('</gpx>')

getgpxfile(name)

"""
#-----------------------------------------------------------------------------------------------
#einbinden der Daten und berechnen der Streckenabscnitte
daten = getdata(name) #np.array(np.genfromtxt("02011548.TXT", comments='#', delimiter='\t'))


xkoord	= latitu(daten[:,2])
ykoord	= longit(daten[:,2],daten[:,1])



#-----------------------------------------------------------------------------------------------
#plotten der Geographischen Daten
fig = figure()
plt.plot(daten[:,1],daten[:,2], 'r-')
plt.title("rohdaten")
plt.xlabel("Longitude [deg]")
plt.ylabel("Latitude [deg]")
plt.axis("equal")
plt.grid("on")
fig.show()

fig = figure()
plt.plot(xkoord,ykoord)
plt.title("Oberflaechenabstaende")
plt.xlabel("Ost-West Strecke [km]")
plt.ylabel("Nord-Ost Strecke [km]")
plt.axis("equal")
plt.grid("on")
plt.show()
"""
