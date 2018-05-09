/*
   -- New project --
   
   This source code of graphical user interface 
   has been generated automatically by RemoteXY editor.
   To compile this code using RemoteXY library 2.3.3 or later version 
   download by link http://remotexy.com/en/library/
   To connect using RemoteXY mobile app by link http://remotexy.com/en/download/                   
     - for ANDROID 4.1.1 or later version;
     - for iOS 1.2.1 or later version;
    
   This source code is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.    
*/

//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// RemoteXY select connection mode and include library 
//#define REMOTEXY_MODE__SOFTSERIAL
#define REMOTEXY_MODE__HARDSERIAL
#include <SoftwareSerial.h>

#include <Braccio.h>
#include <Servo.h>
#include <RemoteXY.h>

// RemoteXY connection settings 
//#define REMOTEXY_SERIAL_RX A0
//#define REMOTEXY_SERIAL_TX A1
#define REMOTEXY_SERIAL Serial1 
#define REMOTEXY_SERIAL_SPEED 9600


// RemoteXY configurate  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =
  { 255,6,0,0,0,51,0,8,13,0,
  4,0,5,7,7,45,2,26,4,0,
  16,7,7,45,2,26,4,0,28,7,
  7,45,2,26,4,0,40,7,7,45,
  2,26,4,0,52,7,7,45,2,26,
  4,0,65,7,7,45,2,26 };
  
// this structure defines all the variables of your control interface 
struct {

    // input variable
  int8_t M1; // =0..100 slider position 
  int8_t M2; // =0..100 slider position 
  int8_t M3; // =0..100 slider position 
  int8_t M4; // =0..100 slider position 
  int8_t M5; // =0..100 slider position 
  int8_t M6; // =0..100 slider position 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////

Servo base;
Servo shoulder;
Servo elbow;
Servo wrist_rot;
Servo wrist_ver;
Servo gripper;

void setup() 
{
  RemoteXY_Init (); 
    Braccio.begin();
  
  // TODO you setup code
   Braccio.ServoMovement(30,         90, 90, 90, 90, 90,  10); 
}

void loop() 
{ 

  
  RemoteXY_Handler ();

int Y1 = map(RemoteXY.M1, 0, 100, 0, 180);
int Y2 = map(RemoteXY.M2, 0, 100, 15, 165);
int Y3 = map(RemoteXY.M3, 0, 100, 0, 180);
int Y4 = map(RemoteXY.M4, 0, 100, 0, 180);
int Y5 = map(RemoteXY.M5, 0, 100, 0, 180);
int Y6 = map(RemoteXY.M6, 0, 100, 10, 73);

  
    Braccio.ServoMovement(30,         Y1, Y2, Y3, Y4, Y5,  Y6);  
  
  // TODO you loop code
  // use the RemoteXY structure for data transfer


}


