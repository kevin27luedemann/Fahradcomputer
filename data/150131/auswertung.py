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


def latitu(x):
	return (x-x[0])*(4e4/360.0)

def longit(x,y):
	return latitu(y)*np.cos(np.pi*(x-x[0])/180.0)

def getdata(name):
	daten = np.array(np.genfromtxt("{}.txt".format(name), comments='#', delimiter='\t'))
	daten[:,1] /= 1e5
	daten[:,2] /= 1e5
	daten[:,3] /= 100.0
	daten[:,4] /= 100.0
	daten[:,0] -= daten[0,0]
	return daten

def getgpxfile(name):
	aus = open("{}.gpx".format(name), "w")
	daten = getdata(name)
	#erstellen des Header
	aus.write('<?xml version="1.0" ?>\n')
	aus.write('<gpx \n\tversion="1.0" \n\tcreator="Kevin Luedemann">\n')
	aus.write('\t<trk>\n')
	aus.write("\t\t<name>{}</name>\n".format(name))
	aus.write('\t\t<trkseg>\n')
	
	#bearbeiten der Punkte in Trackpints fuer das gpx
	for i in range(len(daten[:,0])):
		aus.write("\t\t\t<trkpt lat=\"{}\" lon=\"{}\"> <ele>0</ele> </trkpt>\n".format(daten[i,2],daten[i,1]))

	
	
	#ende der Bereiche
	aus.write('\t\t</trkseg>\n')
	aus.write('\t</trk>\n')
	aus.write('</gpx>')



getgpxfile("ges")




#-----------------------------------------------------------------------------------------------
#einbinden der Daten und berechnen der Streckenabscnitte
daten = getdata("ges") #np.array(np.genfromtxt("02011548.TXT", comments='#', delimiter='\t'))


langengrad	= latitu(daten[:,2])
longitude	= longit(daten[:,2],daten[:,1])


"""
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
plt.plot(longitude,langengrad)
plt.title("Oberflaechenabstaende")
plt.xlabel("Ost-West Strecke [km]")
plt.ylabel("Nord-Ost Strecke [km]")
plt.axis("equal")
plt.grid("on")
plt.show()
"""
