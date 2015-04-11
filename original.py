'''
Created on 30 Jan, 2015

@author: kejie_huang
'''

import matplotlib.pyplot as plt
import serial
import string
from drawnow import *

from time import time

STEParray=[]
STEPLENGTHarray=[]
XARRAY=[0]
YARRAY=[0]
POSITIONX=0
POSITIONY=0
X_offset=0
Y_offset=0
cnt=0
level=1
pressure=1013.25
distance0=0
distance1=0
STEP=0
altitude=0

plt.ion()
plt.show()
#fig = plt.figure()

def makeFig(): #create a function that makes our desired plot
    plt.xlim(-10+X_offset,10+X_offset)
    plt.ylim(-10+Y_offset,10+Y_offset) #set y min and max values
    plt.title("INDOOR POSITIONING SYSTEM") #plot the title
    plt.grid(True);  #turn the grid on
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.plot(XARRAY, YARRAY,'ro')
    plt.plot(XARRAY, YARRAY, linewidth=4.0)
    plt.legend(loc='upper left')
    plt.text(-8+X_offset,9+Y_offset,'S',color='red',fontsize=15)
    plt.text(-9+X_offset,8+Y_offset,'E',color='red',fontsize=15)
    plt.text(-8+X_offset,7+Y_offset,'N',color='red',fontsize=15)
    plt.text(-7+X_offset,8+Y_offset,'W',color='red',fontsize=15)
    #plt.text(8,9,'LV='.format(TEXT))
    plt.text(6+X_offset,9.25+Y_offset,'LV=',color='red',fontsize=12)
    plt.text(8.5+X_offset,9.25+Y_offset,level,color='red',fontsize=12)
    plt.text(6+X_offset,8.5+Y_offset,'P=',color='blue',fontsize=12)
    plt.text(7.5+X_offset,8.5+Y_offset,pressure,color='blue',fontsize=12)
    plt.text(6+X_offset,7.75+Y_offset,'STEPs=',color='orange',fontsize=12)
    plt.text(8.5+X_offset,7.75+Y_offset,STEP,color='orange',fontsize=12)
    plt.text(6+X_offset,7+Y_offset,'D(Line)=',color='purple',fontsize=12)
    plt.text(8.5+X_offset,7+Y_offset,distance0,color='Purple',fontsize=12)
    plt.text(6+X_offset,6.25+Y_offset,'D(Walk)=',color='Green',fontsize=12)
    plt.text(8.5+X_offset,6.25+Y_offset,distance1,color='Green',fontsize=12)
    plt.text(6+X_offset,5.5+Y_offset,'Altitude=',color='grey',fontsize=12)
    plt.text(8.5+X_offset,5.5+Y_offset,altitude,color='grey',fontsize=12)
#    plt.annotate('.', xy=(-8, 9), xytext=None,
#            arrowprops=dict(facecolor='black', shrink=0.00))
#    plt.annotate('.', xy=(-9, 8), xytext=None,
#            arrowprops=dict(facecolor='black', shrink=0.00))
#    plt.annotate('.', xy=(-7, 8), xytext=None,
#            arrowprops=dict(facecolor='black', shrink=0.00))
#    plt.annotate('.', xy=(-8, 7), xytext=None,
#            arrowprops=dict(facecolor='black', shrink=0.00))

# Check your COM port and baud rate
ser = serial.Serial(port='COM4',baudrate=115200, parity=serial.PARITY_NONE, timeout=0)

f = open("Serial"+str(time())+".txt", 'w')


while 1:
    line = ser.readline()
    line.rstrip('\r\n')
#    if line.find("!ANG:") != -1:          # filter out incomplete (invalid) lines
#        line = line.replace("!ANG:","")   # Delete "!ANG:"
    print line
    f.write(line)                     # Write to the output log file
    words = string.split(line,",")    # Fields split
    if len(words) > 2:
        try:
            STEP = float(words[0])
            STEParray.append(STEP)
            STEPLENGTH = float(words[1])
            STEPLENGTHarray.append(STEPLENGTH)
            POSITIONX = float(words[2])
            XARRAY.append(POSITIONX)
            POSITIONY = float(words[3])
            YARRAY.append(POSITIONY)
            X_offset=POSITIONX
            Y_offset=POSITIONY
            #print XARRAY,YARRAY
            distance0=float(words[4])
            distance1=float(words[5])
            pressure=float(words[6])
            altitude=float(words[7])
            level=float(words[8])
        except:
            print "Invalid line"
    #            time.sleep(0.00001)    
        plt.pause(.000001)#sthash.l3R0774z.dpuf
        cnt = cnt + 1
        if(cnt>200):
            STEParray.pop(0)
            STEPLENGTHarray.pop(0)
            XARRAY.pop(0)
            YARRAY.pop(0)
    drawnow(makeFig)
               
ser.close
f.close