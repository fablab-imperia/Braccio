#include <Nunchuk.h>
#include <Wire.h>
#include <Braccio.h>
#include <Servo.h>




struct PosaBraccio 
{
   int angoloServoBase;
   int angoloServoShoulder;
   int angoloServoElbow;
   int angoloServoWristRot;
   int angoloServoWristVer;
   int angoloServoGripper ;

    PosaBraccio()
    {
      reset();
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
    Braccio.begin();
    Wire.begin();
    nunchuk_init();
    
    applica_posa(30,posaCorrente); 
}

void loop() 
{ 

 
  delay(10); 

}
