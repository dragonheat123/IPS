#include <LPS.h>
#include <Wire.h>
#include <LSM303.h>
#include <L3G.h>

L3G gyro;
LPS ps;
LSM303 compass;

float F0, F1, F2, F3, F4, F5, F6, F7;
float pressure;
float altitude;
float temperature;
int level;
int stp = 1;

float bearing =0;
int state = 0;
int heading_0;
int heading_1;
float gyrosum;
float gyrosum1;
float gyroangle1;
float gyroangle;
double steplength = 0;
double steptotal =0;

float y0;
float y1;
float y2;

  int sampleNum=500;
  int dc_offset=0;
  int dc_offset1=0;
  float gyroz;
  float gyroy;
  double noise=0;
  double noise1=0;

void setup() {
  // put your setup code here, to run once:
   Serial.begin(115200);
   Wire.begin();
   compass.init();
   compass.enableDefault();
   compass.m_min = (LSM303::vector<int16_t>){-32767, -32767, -32767};
  compass.m_max = (LSM303::vector<int16_t>){+32767, +32767, +32767};
  gyro.init();
  gyro.enableDefault();
  
//Calculate initial DC offset and noise level of gyro

for(int n=0;n<sampleNum;n++){
 gyro.read();
 dc_offset+=(int)(gyro.g.z*8.75/1000);
 dc_offset1+=(int)(gyro.g.y*8.75/1000);
 delay(10);
}
dc_offset=dc_offset/sampleNum;
dc_offset1=dc_offset1/sampleNum;
Serial.println();
Serial.print("DC Offset: ");
Serial.print(dc_offset);
Serial.println();

Serial.println();
Serial.print("DC Offset1: ");
Serial.print(dc_offset1);
Serial.println();

for(int n=0;n<sampleNum;n++){
 gyro.read();
 if((int)gyro.g.z*8.75/1000-dc_offset>noise)
 noise=(int)gyro.g.z*8.75/1000-dc_offset;
 else if((int)gyro.g.z*8.75/1000-dc_offset<-noise)
 noise=-(int)gyro.g.z*8.75/1000-dc_offset;
 if((int)gyro.g.y*8.75/1000-dc_offset1>noise1)
 noise1=(int)gyro.g.y*8.75/1000-dc_offset1;
 else if((int)gyro.g.y*8.75/1000-dc_offset1<-noise1)
 noise1=-(int)gyro.g.y*8.75/1000-dc_offset1;
 
 
}
//noise=noise/100; //gyro returns hundredths of degrees/sec
//print dc offset and noise level
Serial.println();
Serial.print("DC Offset: ");
Serial.print(dc_offset);
Serial.print("\tNoise Level: ");
Serial.print(noise);
Serial.println();

Serial.println();
Serial.print("DC Offset1: ");
Serial.print(dc_offset);
Serial.print("\tNoise Level1: ");
Serial.print(noise);
Serial.println();


float pressure = ps.readPressureMillibars();
  float altitude = ps.pressureToAltitudeMeters(pressure);
  float temperature = ps.readTemperatureC();
  
  int startlevel = 4;             //////////////////////////////////////////////CHANGE THIS BEFORE STARTING////////////////////////
  float currLevel = startlevel;
  float currAlt = ps.pressureToAltitudeMeters(pressure);
  float levelGap = 4.26;
  float levelGap12 = 5.8;
  
  if ( startlevel == 1 ) {F0= currAlt - levelGap/2;}
  else if ( startlevel == 2 ) {F0= currAlt - levelGap/2 - levelGap12;}
  else if ( startlevel == 3 ) {F0= currAlt - levelGap/2 - levelGap12 - levelGap;}
  else if ( startlevel == 4 ) {F0= currAlt - levelGap/2 - levelGap12 - levelGap*2;}
  else if ( startlevel == 5 ) {F0= currAlt - levelGap/2 - levelGap12 - levelGap*3;}
  else if ( startlevel == 6 ) {F0= currAlt - levelGap/2 - levelGap12 - levelGap*4;}
  else if ( startlevel == 7 ) {F0= currAlt - levelGap/2 - levelGap12 - levelGap*5;}
  
  F1 = F0 + levelGap/2 + levelGap12/2;
  F2 = F1 + levelGap12/2 + levelGap/2;
  F3 = F2 + levelGap;
  F4 = F3 + levelGap;
  F5 = F4 + levelGap;
  F6 = F5 + levelGap;
  F7 = F6 + levelGap;

  if (!ps.init())
  {
    Serial.println("Failed to autodetect pressure sensor!");
    while (1);
  }

  ps.enableDefault();
}

void loop() {
  /////Compass
  compass.read();
  float heading = compass.heading();
  float heading2 = compass.heading((LSM303::vector<int>){0, 1, 0});
  /////Gyro
  gyro.read();
 ///////Pressure    
 pressure = ps.readPressureMillibars();
 altitude = ps.pressureToAltitudeMeters(pressure);
 temperature = ps.readTemperatureC();

 level=0;
  
  if (altitude > F0 && altitude <F1){
    level = 1;
  }
  else if (altitude > F1 && altitude <F2){
    level = 2;
  }
  else if (altitude > F2 && altitude <F3){
    level = 3;
  }
  else if (altitude > F3 && altitude <F4){
    level = 4;
  }
  else if (altitude > F4 && altitude <F5){
    level = 5;
  }
  else if (altitude > F5 && altitude <F6){
    level = 6;
  }
  else if (altitude > F6 && altitude <F7){
    level = 7;
  }
///////////Calculations

     
     
     y0 = y1;
     y1 = y2;
     y2 = compass.a.y/300;
     
     gyroz = (gyro.g.z)*8.75/1000 - dc_offset;
     gyroy = (gyro.g.y)*8.75/1000 - dc_offset1;
     
     if (abs(gyroz)<abs(noise)){
      gyroz =0;
     }
     
     if (abs(gyroy)<abs(noise1)){
      gyroy =0;
     }
     
     if (state == 0){
       heading_0 = heading;
       state =1;
     };
     
     if (state ==1){
       gyrosum1 += gyroy;
//       Serial.println(gyroy);
//       Serial.print("gyrosum1:");
//       Serial.println(gyrosum1);
       if (gyroz>0){
       gyrosum += gyroz;
       
       if (gyrosum>8000){
         gyrosum =0;
       };
       };
       gyroangle1 = gyrosum1*0.02;
       gyroangle = gyrosum *0.02;
//       Serial.println(gyroangle1);
       if (y1+1<y0 && y2>y0+1){
         state = 2;
         steplength = 2*sin((gyroangle+10)/180*3.142/2);
         if (steplength<0.3){
           state=1; 
         }
       }
     };
     
     
     if (state == 2){

//         Serial.println("heading:");
//         Serial.println(gyrosum1);
//         Serial.println("impact");
//         Serial.println("steplength:");
//         Serial.println(steplength);
//         Serial.println("steptotal:");
//        steptotal +=steplength;
//        Serial.print(steptotal);
//        Serial.print(",");
        Serial.print(gyroangle);
        Serial.print(",");
        Serial.print(-gyroangle1);
        Serial.print(",");
        Serial.print(pressure);
        Serial.print(",");
        Serial.print(altitude);
        Serial.print(",");
        Serial.print(temperature);
        Serial.print(",");
        Serial.println(level);
         
        gyrosum =0;
        gyrosum1 =0;
        gyroangle1 =0;
        gyroangle =0;
        delay(500);
        state = 1;
     };


      delay(20);
}
