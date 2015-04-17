
import matplotlib.pyplot as plt
import serial
import string
from drawnow import *
import math 
from time import time
from threading import Thread 

pressure =0
altitude =0
tempertaure =0
level =0
gyroangle=0
gyroangle1=0

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
STEP =0
altitude=0


plt.ion()
plt.show()
#fig = plt.figure()

##leglength() = float(1)


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
    plt.text(6+X_offset,9.25+Y_offset,'LEVEL=',color='red',fontsize=12)
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
ser = serial.Serial(port='COM4',baudrate=115200, parity=serial.PARITY_NONE, timeout= None)

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
        
        gyroangle = float(words[0])*1.2 ##leg angle
        gyroangle1 += float(words[1])    ##directional angle
        pressure = float(words[2])
        altitude = float(words[3])
        temperature = float(words[4])
        level = int(words[5])
    
        STEP += 1
        STEParray.append(STEP)
        STEPLENGTH = 2.5*math.sin(gyroangle/2/180*math.pi)
        STEPLENGTHarray.append(STEPLENGTH)
        POSITIONX += STEPLENGTH*math.cos(gyroangle1/90*math.pi)
        XARRAY.append(POSITIONX)
        POSITIONY+= STEPLENGTH*math.sin(gyroangle1/90*math.pi)
        YARRAY.append(POSITIONY)
        X_offset = POSITIONX
        Y_offset = POSITIONY
        distance0 = sum(STEPLENGTHarray) ##LINE
        distance1 = math.sqrt(POSITIONX*POSITIONX+POSITIONY*POSITIONY) ##WALK
        ##drawnow(makeFig)
        thread = Thread(drawnow(makeFig))
        thread.start()
        thread.join()
        

        
 

        ##time.sleep(0.00001)    
        ##plt.pause(.000001)#sthash.l3R0774z.dpuf
        #cnt = cnt + 1
        #if(cnt>200):
         #   STEParray.pop(0)
          #  STEPLENGTHarray.pop(0)
           # XARRAY.pop(0)
            #YARRAY.pop(0)
    ##drawnow(makeFig)
               
ser.close
f.close


