#include <Wire.h>
#include <LPS.h>

LPS ps;

float F0, F1, F2, F3, F4, F5, F6, F7;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  
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

void loop()
{
  float pressure = ps.readPressureMillibars();
  float altitude = ps.pressureToAltitudeMeters(pressure);
  float temperature = ps.readTemperatureC();

  int level=0;
  
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
  
  Serial.print("p: ");
  Serial.print(pressure);
  Serial.print(" mbar\ta: ");
  Serial.print(altitude);
  Serial.print(" m\tt: ");
  Serial.print(temperature);
  Serial.println(" deg C");
  Serial.print("You are on level ");
  Serial.println(level);
  delay(100);
}
