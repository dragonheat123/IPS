#include <LPS.h>

LPS ps;

float F0, F1, F2, F3, F4, F5, F6, F7;
float pressure;
float altitude;
float temperature;
int level;


// Uncomment the below line to use this axis definition: 
   // X axis pointing forward
   // Y axis pointing to the right 
   // and Z axis pointing down.
// Positive pitch : nose up
// Positive roll : right wing down
// Positive yaw : clockwise
int SENSOR_SIGN[9] = {1,1,1,-1,-1,-1,1,1,1}; //Correct directions x,y,z - gyro, accelerometer, magnetometer
// Uncomment the below line to use this axis definition: 
   // X axis pointing forward
   // Y axis pointing to the left 
   // and Z axis pointing up.
// Positive pitch : nose down
// Positive roll : right wing down
// Positive yaw : counterclockwise
//int SENSOR_SIGN[9] = {1,-1,-1,-1,1,1,1,-1,-1}; //Correct directions x,y,z - gyro, accelerometer, magnetometer

// tested with Arduino Uno with ATmega328 and Arduino Duemilanove with ATMega168

#include <Wire.h>

// LSM303 accelerometer: 8 g sensitivity
// 3.9 mg/digit; 1 g = 256
#define GRAVITY 256  //this equivalent to 1G in the raw data coming from the accelerometer 

#define ToRad(x) ((x)*0.01745329252)  // *pi/180
#define ToDeg(x) ((x)*57.2957795131)  // *180/pi

// L3G4200D gyro: 2000 dps full scale
// 70 mdps/digit; 1 dps = 0.07
#define Gyro_Gain_X 0.07 //X axis Gyro gain
#define Gyro_Gain_Y 0.07 //Y axis Gyro gain
#define Gyro_Gain_Z 0.07 //Z axis Gyro gain
#define Gyro_Scaled_X(x) ((x)*ToRad(Gyro_Gain_X)) //Return the scaled ADC raw data of the gyro in radians for second
#define Gyro_Scaled_Y(x) ((x)*ToRad(Gyro_Gain_Y)) //Return the scaled ADC raw data of the gyro in radians for second
#define Gyro_Scaled_Z(x) ((x)*ToRad(Gyro_Gain_Z)) //Return the scaled ADC raw data of the gyro in radians for second

// LSM303 magnetometer calibration constants; use the Calibrate example from
// the Pololu LSM303 library to find the right values for your board
#define M_X_MIN -187
#define M_Y_MIN -639
#define M_Z_MIN -238
#define M_X_MAX 424
#define M_Y_MAX 295
#define M_Z_MAX 472


//min: { -1334,   -155,  -1611}    max: { -1277,    -68,  -1574}



#define Kp_ROLLPITCH 0.02
#define Ki_ROLLPITCH 0.00002
#define Kp_YAW 1.2
#define Ki_YAW 0.00002

/*For debugging purposes*/
//OUTPUTMODE=1 will print the corrected data, 
//OUTPUTMODE=0 will print uncorrected data of the gyros (with drift)
#define OUTPUTMODE 1

//#define PRINT_DCM 0     //Will print the whole direction cosine matrix
#define PRINT_ANALOGS 0 //Will print the analog raw data
#define PRINT_EULER 1   //Will print the Euler angles Roll, Pitch and Yaw

#define STATUS_LED 13 

float G_Dt=0.02;    // Integration time (DCM algorithm)  We will run the integration loop at 50Hz if possible

long timer=0;   //general purpuse timer
long timer_old;
long timer24=0; //Second timer used to print values 
int AN[6]; //array that stores the gyro and accelerometer data
int AN_OFFSET[6]={0,0,0,0,0,0}; //Array that stores the Offset of the sensors

int gyro_x;
int gyro_y;
int gyro_z;
int accel_x;
int accel_y;
int accel_z;
int magnetom_x;
int magnetom_y;
int magnetom_z;
float c_magnetom_x;
float c_magnetom_y;
float c_magnetom_z;
float MAG_Heading;

float Accel_Vector[3]= {0,0,0}; //Store the acceleration in a vector
float Gyro_Vector[3]= {0,0,0};//Store the gyros turn rate in a vector
float Omega_Vector[3]= {0,0,0}; //Corrected Gyro_Vector data
float Omega_P[3]= {0,0,0};//Omega Proportional correction
float Omega_I[3]= {0,0,0};//Omega Integrator
float Omega[3]= {0,0,0};

// Euler angles
float roll;
float pitch;
float yaw;

float errorRollPitch[3]= {0,0,0}; 
float errorYaw[3]= {0,0,0};

unsigned int counter=0;
byte gyro_sat=0;

float DCM_Matrix[3][3]= {
  {
    1,0,0  }
  ,{
    0,1,0  }
  ,{
    0,0,1  }
}; 
float Update_Matrix[3][3]={{0,1,2},{3,4,5},{6,7,8}}; //Gyros here


float Temporary_Matrix[3][3]={
  {
    0,0,0  }
  ,{
    0,0,0  }
  ,{
    0,0,0  }
};
 
void setup()
{ 
  Serial.begin(115200);
  pinMode (STATUS_LED,OUTPUT);  // Status LED
  
  I2C_Init();

  Serial.println("Pololu MinIMU-9 + Arduino AHRS");

  digitalWrite(STATUS_LED,LOW);
  delay(1500);
 
  Accel_Init();
  Compass_Init();
  Gyro_Init();
  
  delay(20);
  
  for(int i=0;i<32;i++)    // We take some readings...
    {
    Read_Gyro();
    Read_Accel();
    for(int y=0; y<6; y++)   // Cumulate values
      AN_OFFSET[y] += AN[y];
    delay(20);
    }
    
  for(int y=0; y<6; y++)
    AN_OFFSET[y] = AN_OFFSET[y]/32;
    
  AN_OFFSET[5]-=GRAVITY*SENSOR_SIGN[5];
  
  //Serial.println("Offset:");
  for(int y=0; y<6; y++)
    Serial.println(AN_OFFSET[y]);
  
  delay(2000);
  digitalWrite(STATUS_LED,HIGH);
    
  timer=millis();
  delay(20);
  counter=0;
  
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

void loop() //Main Loop
{
  if((millis()-timer)>=100)  // Main loop runs at 50Hz
  {
    counter++;
    timer_old = timer;
    timer=millis();
    if (timer>timer_old)
      G_Dt = (timer-timer_old)/1000.0;    // Real time of loop run. We use this on the DCM algorithm (gyro integration time)
    else
      G_Dt = 0;
    
    // *** DCM algorithm
    // Data adquisition
    Read_Gyro();   // This read gyro data
    Read_Accel();     // Read I2C accelerometer
    
    if (counter > 5)  // Read compass data at 10Hz... (5 loop runs)
      {
      counter=0;
      Read_Compass();    // Read I2C magnetometer
      Compass_Heading(); // Calculate magnetic heading  
      }
    
    // Calculations...
    Matrix_update(); 
    Normalize();
    Drift_correction();
    Euler_angles();
    // ***
    
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
   
    printdata();
  }
   
}
