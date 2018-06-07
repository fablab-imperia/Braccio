#include <Braccio.h>
#include <Servo.h>

#include <ArduinoJson.h>

#define Bluetooth Serial1
#define BUFFER_SIZE 256

#define POSA1 0
#define POSA2 1
#define POSA3 2
#define POSA4 3
#define POSA5 4
#define POSA6 5

#define REALTIME_MODE 0
#define PLAYBACK_MODE 1


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

PosaBraccio pose_salvate[6];

int current_mode = REALTIME_MODE;
int current_playback_pose = POSA1;
bool  poseCrescenti = true;

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

      if (str == "esegui") 
      {
             if (current_mode == REALTIME_MODE)
              {
                current_mode = PLAYBACK_MODE;
                Serial.println("Modalita playback attivata");
              }
              else if (current_mode == PLAYBACK_MODE)
              {  
                current_mode = REALTIME_MODE;
                Serial.println("Modalita realtime attivata");
              }
      } 
      else if (str == "reset") 
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
      
          if (str.indexOf("s") == 0) 
          {
            int posizione = str.substring(1).toInt();
            Serial.print("salvo Posizione ");
            Serial.println(posizione);
            salva_posa(posizione, posaCorrente.angoloServoBase, 
                                  posaCorrente.angoloServoShoulder, 
                                  posaCorrente.angoloServoElbow, 
                                  posaCorrente.angoloServoWristRot, 
                                  posaCorrente.angoloServoWristVer, 
                                  posaCorrente.angoloServoGripper); 
          }       
          //Se ricevo JSON, lo leggo e interpreto
          else if (str.indexOf('{') >= 0) 
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
    
            }
            applica_posa(30,posaCorrente);
          }     
    } 
  }
  }

   if (current_mode == PLAYBACK_MODE) 
   {   
      //playback
        posa_seguente();
   }
 
  delay(10); 

}
