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

#define POSA1 0
#define POSA2 1
#define POSA3 2
#define POSA4 3
#define POSA5 4
#define POSA6 5

#include <Braccio.h>
#include <Servo.h>
#include <RemoteXY.h>

// RemoteXY connection settings 
//#define REMOTEXY_SERIAL_RX A0
//#define REMOTEXY_SERIAL_TX A1
#define REMOTEXY_SERIAL Serial1 
#define REMOTEXY_SERIAL_SPEED 9600


#define REALTIME_MODE 0
#define PLAYBACK_MODE 1


// RemoteXY configurate  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =
  { 255,14,0,0,0,202,0,8,13,0,
  1,0,82,5,5,5,2,31,80,49,
  0,1,0,82,13,5,5,2,31,80,
  50,0,1,0,82,21,5,5,2,31,
  80,51,0,1,0,82,29,5,5,2,
  31,80,52,0,1,0,82,37,5,5,
  2,31,80,53,0,1,0,82,46,5,
  5,2,31,80,54,0,1,0,81,54,
  7,7,12,31,62,0,4,0,9,4,
  8,43,2,26,4,0,22,4,8,43,
  2,26,4,0,35,4,8,43,2,26,
  4,0,48,4,8,43,2,26,4,0,
  62,4,8,43,2,26,1,0,90,54,
  7,7,36,31,88,0,129,0,33,57,
  11,4,17,48,45,49,56,48,194,176,
  0,4,128,9,51,63,7,2,26,129,
  0,10,3,6,4,17,77,50,0,129,
  0,23,3,6,4,17,77,51,0,129,
  0,36,3,6,4,17,77,52,0,129,
  0,49,3,6,4,17,77,53,0,129,
  0,63,3,6,4,17,77,54,0 };
  
// this structure defines all the variables of your control interface 
struct {

    // input variable
  uint8_t P1; // =1 if button pressed, else =0 
  uint8_t P2; // =1 if button pressed, else =0 
  uint8_t P3; // =1 if button pressed, else =0 
  uint8_t P4; // =1 if button pressed, else =0 
  uint8_t P5; // =1 if button pressed, else =0 
  uint8_t P6; // =1 if button pressed, else =0 
  uint8_t play; // =1 if button pressed, else =0 
  int8_t M2; // =0..100 slider position 
  int8_t M3; // =0..100 slider position 
  int8_t M4; // =0..100 slider position 
  int8_t M5; // =0..100 slider position 
  int8_t M6; // =0..100 slider position 
  uint8_t cancel; // =1 if button pressed, else =0 
  int8_t M1; // =0..100 slider position 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY, LastRemoteXY;
#pragma pack(pop)

struct PosaBraccio 
{
   int angoloServoBase = -1;
   int angoloServoShoulder = -1;
   int angoloServoElbow = -1;
   int angoloServoWristRot = -1;
   int angoloServoWristVer = -1;
   int angoloServoGripper = -1;


    void reset() 
    {
       angoloServoBase = -1;
       angoloServoShoulder = -1;
       angoloServoElbow = -1;
       angoloServoWristRot = -1;
       angoloServoWristVer = -1;
       angoloServoGripper = -1;
    }

    bool IsInvalid() 
    {
      return angoloServoBase == -1 || 
             angoloServoShoulder == -1 ||
             angoloServoElbow == -1 ||
             angoloServoWristRot == -1 ||
             angoloServoWristVer == -1 ||
             angoloServoGripper == -1;
    }
    
};

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////



Servo base;
Servo shoulder;
Servo elbow;
Servo wrist_rot;
Servo wrist_ver;
Servo gripper;

PosaBraccio pose_salvate[6];
int current_mode = REALTIME_MODE;
int current_playback_pose = POSA1;
bool  poseCrescenti = true;

void salva_posa(int numeroPosa, int y1, int y2, int y3, int y4, int y5, int y6) 
{
     if (numeroPosa > 5 || numeroPosa < 0)
      return;
      
     pose_salvate[numeroPosa].angoloServoBase = y1;
     pose_salvate[numeroPosa].angoloServoShoulder = y2;
     pose_salvate[numeroPosa].angoloServoElbow = y3;
     pose_salvate[numeroPosa].angoloServoWristRot = y4;
     pose_salvate[numeroPosa].angoloServoWristVer = y5;
     pose_salvate[numeroPosa].angoloServoGripper = y6;
}

void posa_seguente() 
{ 

    Serial.print("Ultima posa numero ");
    Serial.println(current_playback_pose);
    if (poseCrescenti)
    {
          Serial.print("Incremento frame... valore corrente ");
          Serial.println(current_playback_pose);
          current_playback_pose++;
          while (current_playback_pose < 6 && pose_salvate[current_playback_pose].IsInvalid()) 
          {         
              Serial.print("Salto valore ");
              Serial.println(current_playback_pose);
              current_playback_pose++;
          }

          //sono sono arrivato al limite: devo tornare indietro alla prima posizione buona e invertire il senso dei frame
          if (current_playback_pose == 6)
          {
              
                Serial.println("Sono al limite comincio a decrementare...");
                poseCrescenti = false;
    
                current_playback_pose--;
                Serial.print("Valore corrente ");
                Serial.println(current_playback_pose);
               //continua a decrementare saltando le pose non memorizzate
                while (current_playback_pose >= 0 && pose_salvate[current_playback_pose].IsInvalid()) 
                {
                    Serial.print("Salto valore ");
                    Serial.println(current_playback_pose);
                    current_playback_pose--;
                } 
          }
    }
    else 
    {
          Serial.print("Decremento frame... valore corrente ");
          Serial.println(current_playback_pose);
          current_playback_pose--;
          //continua a decrementare saltando le pose non memorizzate
          while (current_playback_pose >= 0 && pose_salvate[current_playback_pose].IsInvalid()) 
          {
              Serial.print("Salto valore ");
              Serial.println(current_playback_pose);
              current_playback_pose--;
          } 
    
           //sono sono arrivato al limite: devo tornare indietro alla prima posizione buona e invertire il senso dei frame
          if (current_playback_pose < 0)
          {
              
                Serial.println("Sono al limite comincio a incrementare...");
                poseCrescenti = true;

                current_playback_pose++;
                Serial.print("Valore corrente ");
                Serial.println(current_playback_pose);
               //continua a decrementare saltando le pose non memorizzate
                while (current_playback_pose < 6 && pose_salvate[current_playback_pose].IsInvalid()) 
                {
                    Serial.print("Salto valore ");
                    Serial.println(current_playback_pose);
                    current_playback_pose++;
                }
          }
    }
    
    if (current_playback_pose < 0)
      current_playback_pose = 0;
     
    if (current_playback_pose > 5)
      current_playback_pose = 5;  

    Serial.print("PROSSIMA POSA DA ASSUMERE: ");
    Serial.println(current_playback_pose);
    applica_posa(30, pose_salvate[current_playback_pose]);
}

void applica_posa(int speed, PosaBraccio& posa) 
{
    
    Serial.println("Playback mode... ");
    Serial.print("Posizione (");
    Serial.print(posa.angoloServoBase);
    Serial.print(",");
    Serial.print( posa.angoloServoShoulder);
    Serial.print(",");
    Serial.print(posa.angoloServoElbow);
    Serial.print(",");
    Serial.print(posa.angoloServoWristRot);
    Serial.print(",");
    Serial.print(posa.angoloServoWristVer);
    Serial.print(",");
    Serial.print(posa.angoloServoGripper);
    Serial.print(")");
    Braccio.ServoMovement(speed,  posa.angoloServoBase, 
                                  posa.angoloServoShoulder, 
                                  posa.angoloServoElbow, 
                                  posa.angoloServoWristRot,
                                  posa.angoloServoWristVer,
                                  posa.angoloServoGripper);  
}

void setup() 
{
    Serial.begin(9600);
    RemoteXY_Init (); 
    Braccio.begin();
   // Braccio.ServoMovement(30,         90, 90, 90, 90, 90,  10); 
}

void loop() 
{ 

  LastRemoteXY.P1 = RemoteXY.P1;
  LastRemoteXY.P2 = RemoteXY.P2;
  LastRemoteXY.P3 = RemoteXY.P3;
  LastRemoteXY.P4 = RemoteXY.P4;
  LastRemoteXY.P5 = RemoteXY.P5;
  LastRemoteXY.P6 = RemoteXY.P6;
  LastRemoteXY.play = RemoteXY.play;
  LastRemoteXY.M2 = RemoteXY.M2;
  LastRemoteXY.M3 = RemoteXY.M3;
  LastRemoteXY.M4 = RemoteXY.M4;
  LastRemoteXY.M5 = RemoteXY.M5;
  LastRemoteXY.M6 = RemoteXY.M6;
  LastRemoteXY.M1 = RemoteXY.M1;
  LastRemoteXY.cancel = RemoteXY.cancel;
  
  RemoteXY_Handler ();

  int Y1 = map(RemoteXY.M1, 0, 100, 0, 180);
  int Y2 = map(RemoteXY.M2, 0, 100, 15, 165);
  int Y3 = map(RemoteXY.M3, 0, 100, 0, 180);
  int Y4 = map(RemoteXY.M4, 0, 100, 0, 180);
  int Y5 = map(RemoteXY.M5, 0, 100, 0, 180);
  int Y6 = map(RemoteXY.M6, 0, 100, 10, 73);

 if (LastRemoteXY.play != RemoteXY.play && RemoteXY.play == 1) 
 {
      if (current_mode == REALTIME_MODE)
      {
        current_mode = PLAYBACK_MODE;
        Serial.println("Modalita playback attivata");
      }
      else 
      {  
        current_mode = REALTIME_MODE;
        Serial.println("Modalita realtime attivata");
      }
 } 
  else if (LastRemoteXY.cancel != RemoteXY.cancel && RemoteXY.cancel == 1) 
  {
     Serial.println("Reset delle posizioni!");
     pose_salvate[0].reset();
     pose_salvate[1].reset();
     pose_salvate[2].reset();
     pose_salvate[3].reset();
     pose_salvate[4].reset();
     pose_salvate[5].reset();
     current_mode = REALTIME_MODE;
  }

      if (current_mode == REALTIME_MODE)
      {
      
           if (LastRemoteXY.P1 != RemoteXY.P1 &&  RemoteXY.P1 == 1) 
           {
                Serial.println("Posa 1 memorizzata");
                salva_posa(POSA1, Y1, Y2, Y3, Y4, Y5, Y6);  
           } 
           else if (LastRemoteXY.P2 != RemoteXY.P2 &&  RemoteXY.P2 == 1) 
           {
                Serial.println("Posa 2 memorizzata");
                salva_posa(POSA2, Y1, Y2, Y3, Y4, Y5, Y6);  
           }
           else if (LastRemoteXY.P3 != RemoteXY.P3 && RemoteXY.P3 == 1) 
           {
                Serial.println("Posa 3 memorizzata");
                salva_posa(POSA3, Y1, Y2, Y3, Y4, Y5, Y6);  
           }
           else if (LastRemoteXY.P4 != RemoteXY.P4 && RemoteXY.P4 == 1) 
           {
                Serial.println("Posa 4 memorizzata");
                salva_posa(POSA4, Y1, Y2, Y3, Y4, Y5, Y6);  
           }
           else if (LastRemoteXY.P5 != RemoteXY.P5 && RemoteXY.P5 == 1) 
           {
                Serial.println("Posa 5 memorizzata");
                salva_posa(POSA5, Y1, Y2, Y3, Y4, Y5, Y6);  
           }
           else if (LastRemoteXY.P6 != RemoteXY.P6 && RemoteXY.P6 == 1) 
           {
                Serial.println("Posa 6 memorizzata");
                salva_posa(POSA6, Y1, Y2, Y3, Y4, Y5, Y6);  
           }         
           else 
           {
            
                  Braccio.ServoMovement(30, Y1, Y2, Y3, Y4, Y5,  Y6); 
            
           }
      }

     if (current_mode == PLAYBACK_MODE) 
     {   
        //playback
          posa_seguente();
     }
 
}
