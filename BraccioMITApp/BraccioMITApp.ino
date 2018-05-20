#include <Braccio.h>
#include <Servo.h>

#include <ArduinoJson.h>

#define Bluetooth Serial1
#define BUFFER_SIZE 256

struct PosaBraccio 
{
   int angoloServoBase;
   int angoloServoShoulder;
   int angoloServoElbow;
   int angoloServoWristRot;
   int angoloServoWristVer;
   int angoloServoGripper;

    PosaBraccio()
    {
      
    }

    PosaBraccio(int m1,int m2, int m3, int m4, int m5, int m6)
    {
          this->angoloServoBase = m1;
          this->angoloServoShoulder = m2;
          this->angoloServoElbow = m3;
          this->angoloServoWristRot = m4;
          this->angoloServoWristVer = m5;
          this->angoloServoGripper = m6;   
    }

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


Servo base;
Servo shoulder;
Servo elbow;
Servo wrist_rot;
Servo wrist_ver;
Servo gripper;

PosaBraccio posaCorrente;

PosaBraccio pose_salvate[6];


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

void applica_posa(int speed, PosaBraccio& posa) 
{
    Braccio.ServoMovement(speed,  posa.angoloServoBase, 
                                  posa.angoloServoShoulder, 
                                  posa.angoloServoElbow, 
                                  posa.angoloServoWristRot,
                                  posa.angoloServoWristVer,
                                  posa.angoloServoGripper);  
}

void setup() 
{

    posaCorrente.angoloServoBase  = 90;
    posaCorrente.angoloServoShoulder  = 90;
    posaCorrente.angoloServoElbow  = 90;
    posaCorrente.angoloServoWristRot  = 90;
    posaCorrente.angoloServoWristVer  = 90;
    posaCorrente.angoloServoGripper  = 10;

    Serial.begin(9600);
    Bluetooth.begin(9600);
     
    Braccio.begin();
    Braccio.ServoMovement(30,         90, 90, 90, 90, 90,  10); 
}

void loop() 
{ 

  char buffer[BUFFER_SIZE];
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  
  while (Bluetooth.available() > 0)
  { 
    // Checks whether data is comming from the serial port
    int numeroCaratteri = Bluetooth.readBytesUntil('!',buffer, BUFFER_SIZE); // Reads the data from the serial port
    if (numeroCaratteri > 0 ) 
    {
      buffer[numeroCaratteri] = '\0';
      String str(buffer);
      Serial.println(str);  

      //Se ricevo JSON, lo leggo e interpreto
      if (str.indexOf('{') >= 0) 
      {    
        JsonObject& root = jsonBuffer.parseObject(buffer); 
        if (root.success()) 
        {
            if (root.containsKey("m1")) 
            {
              int m1 = root["m1"];
              posaCorrente.angoloServoBase = m1;
            }
            else if (root.containsKey("m2")) 
            {
              int m2 = root["m2"];
              posaCorrente.angoloServoShoulder = m2;
            }    
            else if (root.containsKey("m3")) 
            {
              int m3 = root["m3"];
              posaCorrente.angoloServoElbow = m3;
            }
            else if (root.containsKey("m4")) 
            {
              int m4 = root["m4"];
              posaCorrente.angoloServoWristRot = m4;
            }
            else if (root.containsKey("m5")) 
            {
              int m5 = root["m5"];
              posaCorrente.angoloServoWristVer = m5;
            } 
            else if (root.containsKey("m6")) 
            {
              int m6 = root["m6"];
              posaCorrente.angoloServoGripper = m6;
            }

            applica_posa(30,posaCorrente); 
        }
        
      } 
      else if (str == "salva") 
      {
        
      } 
      else if (str == "esegui") 
      {
      
      } 
      else if (str == "reset") 
      {
      }
    }
  }
 
  delay(50);

 /* int Y1 = map(1, 0, 100, 0, 180);
  int Y2 = map(2, 0, 100, 15, 165);
  int Y3 = map(3, 0, 100, 0, 180);
  int Y4 = map(4, 0, 100, 0, 180);
  int Y5 = map(65, 0, 100, 0, 180);
  int Y6 = map(7, 0, 100, 10, 73);*/


    //  salva_posa(1,  Y1, Y2, Y3, Y4, Y5,  Y6);  
 
  
   //
//   Braccio.ServoMovement(30,         Y1, Y2, Y3, Y4, Y5,  Y6);  

}
