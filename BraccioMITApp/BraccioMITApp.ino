#include <Nunchuk.h>
#include <Wire.h>

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
#define DEMO_MODE 2
#define NUNCHUK_MODE 3

#define NUNCHUK_SOGLIA_X_POS 90
#define NUNCHUK_SOGLIA_X_NEG -80
#define NUNCHUK_SOGLIA_Y_POS 90
#define NUNCHUK_SOGLIA_Y_NEG -90

#define NUNCHUK_ROLL_POS 1.0f
#define NUNCHUK_ROLL_NEG -1.0f
#define NUNCHUK_PITCH_POS 0.8f
#define NUNCHUK_PITCH_NEG -0.6f

#define NUNCHUK_X_SPEED 3
#define NUNCHUK_Y_SPEED 5
#define NUNCHUK_Y_SHOULDER_SPEED 1

#define NUNCHUK_ROLL_SPEED 5
#define NUNCHUK_PITCH_SPEED 5
#define NUNCHUK_GRIPPER_SPEED 5



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

    void setPosa(int m1,int m2, int m3, int m4, int m5, int m6)
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

PosaBraccio pose_demo[8];

int current_mode = REALTIME_MODE;
int current_playback_pose = POSA1;
int current_playback_pose_demo = POSA3;
bool  poseCrescenti = true;
bool primo_giro_demo = true;

void stampa_posa(PosaBraccio& posa) 
{
    Serial.println(F("POSA CORRENTE: ")); 
    Serial.print(F("m1: "));
    Serial.println(posa.angoloServoBase); 
    Serial.print(F("m2: ")); 
    Serial.println(posa.angoloServoShoulder); 
    Serial.print(F("m3: ")); 
    Serial.println(posa.angoloServoElbow);
    Serial.print(F("m4: ")); 
    Serial.println(posa.angoloServoWristRot);
    Serial.print(F("m5: ")); 
    Serial.println(posa.angoloServoWristVer);
    Serial.print(F("m6: "));  
    Serial.println(posa.angoloServoGripper);
  
}


void run_demo() 
{
    
    if( poseCrescenti) 
    {
        current_playback_pose_demo++;
        if (current_playback_pose_demo > 7)  
        {
          poseCrescenti = false;
          current_playback_pose_demo = 6;
        }
    } 
    else 
    {
        current_playback_pose_demo--;
        if (current_playback_pose_demo < 0 )  
        {
          poseCrescenti = true;
          current_playback_pose_demo = 1;
        }
    }

    
    applica_posa(40,  pose_demo[current_playback_pose_demo]); 
    primo_giro_demo = false;
    
    
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

void carica_pose_demo() 
{
    pose_demo[0].setPosa(0,76,23,41,90,10);
    pose_demo[1].setPosa(0,76,23,41,90,56);
    pose_demo[2].setPosa(0,105,23,31,90,56);
    pose_demo[3].setPosa(108,128,23,13,90,56);
    pose_demo[4].setPosa(180,128,23,13,90,56);
    pose_demo[5].setPosa(180,82,23,49,90,56);
    pose_demo[6].setPosa(180,76,23,40,90,56);
    pose_demo[7].setPosa(180,76,23,41,90,10);  
}

void handle_nunchuk() 
{
      if (nunchuk_read()) 
      {
          int x = nunchuk_joystickX();
          int y = nunchuk_joystickY();
          float pitch = nunchuk_pitch();
          float roll = nunchuk_roll();

          boolean apriPinza = nunchuk_buttonZ();
          boolean chiudiPinza = nunchuk_buttonC();
          
          Serial.print("PITCH = ");
          Serial.println(pitch);
          Serial.print("ROLL = ");
          Serial.println(roll);

          

          if (x > NUNCHUK_SOGLIA_X_POS) 
          {
              if (posaCorrente.angoloServoBase < 180)
              {
                posaCorrente.angoloServoBase += NUNCHUK_X_SPEED;
              }
          }

           if (x < NUNCHUK_SOGLIA_X_NEG) 
          {
              if (posaCorrente.angoloServoBase > 0)
              {
                posaCorrente.angoloServoBase -= NUNCHUK_X_SPEED;
              }
          }

          if (y > NUNCHUK_SOGLIA_Y_POS) 
          {
              if (posaCorrente.angoloServoElbow < 180)
              {
                posaCorrente.angoloServoElbow += NUNCHUK_Y_SPEED;
                posaCorrente.angoloServoShoulder += NUNCHUK_Y_SHOULDER_SPEED; 
              
              }
          }

          if (y < NUNCHUK_SOGLIA_Y_NEG) 
          {
              if (posaCorrente.angoloServoElbow > 0)
              {
                posaCorrente.angoloServoElbow -= NUNCHUK_Y_SPEED;
                posaCorrente.angoloServoShoulder -= NUNCHUK_Y_SHOULDER_SPEED; 
              }
          }

          if (roll > NUNCHUK_ROLL_POS) 
          {
              if (posaCorrente.angoloServoWristVer > 0)
              {
                posaCorrente.angoloServoWristVer -= NUNCHUK_ROLL_SPEED;
              }
          }

           if (roll < NUNCHUK_ROLL_NEG) 
          {
              if (posaCorrente.angoloServoWristVer < 180)
              {
                posaCorrente.angoloServoWristVer += NUNCHUK_ROLL_SPEED;
              }
          }

          if (pitch > NUNCHUK_PITCH_POS) 
          {
              if (posaCorrente.angoloServoWristRot > 0)
              {
              posaCorrente.angoloServoWristRot -= NUNCHUK_PITCH_SPEED; 
              }
          }

          if (pitch < NUNCHUK_PITCH_NEG) 
          {
              if (posaCorrente.angoloServoWristRot < 180)
              {
                posaCorrente.angoloServoWristRot += NUNCHUK_PITCH_SPEED;
              }
          }

         if (apriPinza)
         {
              if (posaCorrente.angoloServoGripper > 10)
              {
                posaCorrente.angoloServoGripper -= NUNCHUK_GRIPPER_SPEED;
              }
              
         } 
         else if (chiudiPinza) 
         {
              if (posaCorrente.angoloServoGripper < 73)
              {
                posaCorrente.angoloServoGripper += NUNCHUK_GRIPPER_SPEED;
              }
         } 
         
          

          applica_posa(50, posaCorrente);
          
      }
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
    Wire.begin();
    nunchuk_init();
       
    Braccio.begin();
    Braccio.ServoMovement(30,         90, 90, 90, 90, 90,  10); 

  
    carica_pose_demo();
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

      if (current_mode == DEMO_MODE && (str == "normal" || str == "nunchuk")) 
      {
         applica_posa(30,pose_demo[3]);
         delay(500);
         applica_posa(30,pose_demo[2]);
         delay(500);
         applica_posa(30,pose_demo[1]);
         delay(500);
         applica_posa(30,pose_demo[0]);
         applica_posa(30,pose_demo[2]);
         
      }

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
      else if (str == "demo") 
      {
          poseCrescenti = true;
          primo_giro_demo = true;
          current_playback_pose_demo = -1;
          current_mode = DEMO_MODE;

          PosaBraccio posa_iniziale_demo(0,90,32,90,90,10);   
          applica_posa(30,posa_iniziale_demo);
          delay(500);

          
      }
      else if (str == "normal") 
      {
          poseCrescenti = true;
          current_mode = REALTIME_MODE;
      }
      else if (str == "nunchuk") 
      {
          current_mode = NUNCHUK_MODE;
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
          }
        applica_posa(30,posaCorrente);
       }     
     }
   }
   
   if (current_mode == PLAYBACK_MODE) 
   {   
      //playback
        posa_seguente();
   }

   if (current_mode == DEMO_MODE) 
   {        
        run_demo();
   }

   if (current_mode == NUNCHUK_MODE) 
   {        
        handle_nunchuk();
   }
    

 
  delay(10); 

}
