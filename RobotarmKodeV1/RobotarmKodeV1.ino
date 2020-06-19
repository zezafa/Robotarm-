#include <Servo.h> 
#include "IRremote.h"

int receiver = 8;
int ServoBasePin = 9;
int ServoArm1Pin = 10;
int ServoArm2Pin = 11;
int ServoArm3Pin = 12;
int ServoGribPin = 13;
int Valg = 2;                         // 1: Robotarm med kode    2: Robotarm ved IR

Servo ServoBase;
Servo ServoArm1;  
Servo ServoArm2;  
Servo ServoArm3;  
Servo ServoGrib;    
IRrecv irrecv(receiver);     // Biblioteket søger for gemme resultatet i "results". Den retunere 0 hvis der er ingen data. Den retunere 1 hvis dataen er klar
decode_results results;      //...

int PosBase=90;             //Start betingelser ved IR 
int PosArm1=90;
int PosArm2=90;
int PosArm3=90;
int PosGrib=90;
int DegreeToTurnWithIR = 5;

void setup() 
{ 
  ServoBase.attach(ServoBasePin); 
  ServoArm1.attach(ServoArm1Pin); 
  ServoArm2.attach(ServoArm2Pin); 
  ServoArm3.attach(ServoArm3Pin); 
  ServoGrib.attach(ServoGribPin); 
  if (Valg==2){           // IR er valgt
    irrecv.enableIRIn();
    ServoBase.write(90);  
    ServoArm1.write(90); 
    ServoArm2.write(90);  
    ServoArm3.write(90);  
    ServoGrib.write(90);   
   }
} 
 
void loop()
{
if (Valg==1)
{Kode();}

else
{IRRemote();}
}

void Kode()               //Selvvalgt kode uden IR 
{
   ServoBase.write(45);  
   delay(1000);          
   ServoBase.write(0);
   delay(1000);
   ServoBase.write(90);
   delay(1000);
   ServoArm1.write(90);  
   delay(1000);          
   ServoArm1.write(0);
   delay(1000);
   ServoArm1.write(180);
   delay(1000);
}

void IRRemote()         //Fjernstyret med IR
{
    if (irrecv.decode(&results)) // Venter på modtagelse af signal
    {
     translateIR(); 
     irrecv.resume(); // Modtag ny værdi
    }  
}


void translateIR() // Hvad har vi fået ind?
{
  switch(results.value)
  {
  case 0xFF629D:                                                            break;      //FORWARD
  case 0xFF22DD: PosBase=+DegreeToTurnWithIR; ServoBase.write(PosBase);     break;      //LEFT
  case 0xFF02FD:                                                            break;      //OK
  case 0xFFC23D: PosBase=-DegreeToTurnWithIR; ServoBase.write(PosBase);     break;      //RIGHT
  case 0xFFA857:                                                            break;      //REVERSE
  case 0xFF6897: PosArm1=+DegreeToTurnWithIR; ServoBase.write(PosArm1);     break;      //1
  case 0xFF9867: PosArm1=-DegreeToTurnWithIR; ServoBase.write(PosArm1);     break;      //2
  case 0xFFB04F:                                                            break;      //3
  case 0xFF30CF: PosArm2=+DegreeToTurnWithIR; ServoBase.write(PosArm2);     break;      //4
  case 0xFF18E7: PosArm2=-DegreeToTurnWithIR; ServoBase.write(PosArm2);     break;      //5
  case 0xFF7A85:                                                            break;      //6
  case 0xFF10EF: PosArm3=+DegreeToTurnWithIR; ServoBase.write(PosArm3);     break;      //7
  case 0xFF38C7: PosArm3=-DegreeToTurnWithIR; ServoBase.write(PosArm3);     break;      //8
  case 0xFF5AA5:                                                            break;      //9
  case 0xFF42BD: PosGrib=+DegreeToTurnWithIR; ServoBase.write(PosGrib);     break;      //*
  case 0xFF4AB5: PosGrib=-DegreeToTurnWithIR; ServoBase.write(PosGrib);     break;      //0
  case 0xFF52AD:                                                            break;      //#
  case 0xFFFFFFFF:                                                          break;      //Gentag
  default: 
  break;
  }
  delay(500); 
} 
