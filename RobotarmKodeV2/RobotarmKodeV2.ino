#include <Servo.h> 
#include "IRremote.h"

int receiver = 8;
int ServoBasePin = 9;
int ServoArm1Pin = 10;
int ServoArm2Pin = 11;
int ServoArm3Pin = 12;
int ServoGribPin = 13;
int Valg;                         // 1: Robotarm med kode    2: Robotarm ved IR      3: Styring via serial

int PosBase=90;             //Start betingelser ved IR 
int PosArm1=90;
int PosArm2=90; 
int PosArm3=90;
int PosGrib=90;
int DegreeToTurnWithIR;;
int delayWithIR;

Servo ServoBase;
Servo ServoArm1;  
Servo ServoArm2;  
Servo ServoArm3;  
Servo ServoGrib;    
IRrecv irrecv(receiver);     // Biblioteket søger for gemme resultatet i "results". Den retunere 0 hvis der er ingen data. Den retunere 1 hvis dataen er klar
decode_results results;      //...


void setup() 
{ 
  Serial.begin(9600);
  ServoBase.attach(ServoBasePin); 
  ServoArm1.attach(ServoArm1Pin); 
  ServoArm2.attach(ServoArm2Pin); 
  ServoArm3.attach(ServoArm3Pin); 
  ServoGrib.attach(ServoGribPin); 
  Serial.println("Hvordan oensker du at styrer robotarmen? 1: Robotarm styret med kode skrevet paa forhaand, 2: Robotarm styret af fjernbetrjening, 3: Robotarm styret af serial kommunikation med denne PC");
  while (Serial.available() == 0);
  Valg = Serial.parseInt();
  if (Valg==1)
  {
    Serial.println();
    Serial.println("Velkommen til styring af robotarm med kode skrevet på forhaand! Robotarmen vil begynde nu");
  }
  if (Valg==2){           // IR er valgt
     Serial.println();
    Serial.println("Velkommen til styring af robotarm med fjernbetjening. Vaelg venligst foelsomheden (Grader pr tryk):");   //typisk 5
    while (Serial.available() == 0);
    DegreeToTurnWithIR = Serial.parseInt();
    Serial.println("Vaelg min hastighed mellem hvert tryk i ms:");     //Typsik 50ms
    while (Serial.available() == 0); 
    delayWithIR = Serial.parseInt();
    Serial.println("Fjernbetjeningen kan nu benyttes!");
    
    irrecv.enableIRIn();
slowmotor(PosBase,1, 10); slowmotor(PosArm1,2, 10); slowmotor(PosArm2,3, 10); slowmotor(PosArm3,4, 10); slowmotor(PosGrib, 5, 10);  
   }
  if (Valg==3){
    Serial.println();
    Serial.println("Velkommen til styring af robotarmen med PC! Positionen vaelges i grader og ved motoren gaelder: Base=1, Armen taettest paa basen=2, Armen i midten=3, Yderste arm=4, Gribeklo=5. Ved kloen sættes til position 125 er kloen lukket imens den ved 90 er aaben");
  }
} 
 
void loop()
{
if (Valg==1)
{Kode();}
else if(Valg==2)
{IRRemote();}
else {Serialkontakt();}
}

void Kode()               //Selvvalgt kode uden IR 
{
   slowmotor(125,5, 5);        //Drejegrad, motor (Base=1, arm1=2, arm2=3, arm3=4, grib=5), hastighed (0=hurtig, 30=langsomt)    grib=125 (lukket) 50(Åben) 
   delay(500);   
    slowmotor(50,5, 5);      
   delay(500);   
    slowmotor(0,4, 5);      
   delay(500);   
    slowmotor(90,4, 5);   
   delay(500);   
    slowmotor(180,4, 5);    
}

void Serialkontakt()
{
  Serial.write("Hvilken position?");
  while (Serial.available() == 0);
  int Positionvalg = Serial.parseInt();
  Serial.write(" - ");
  Serial.println(Positionvalg);

  Serial.write("Hvilken motor?");
  while (Serial.available() == 0);
  int Motorvalg = Serial.parseInt();
  Serial.write(" - ");
  Serial.println(Motorvalg);
  
  if ((Positionvalg < 181) && (Motorvalg < 6))
  {
    slowmotor(Positionvalg, Motorvalg, 10); 
  }  
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
  case 0xFF629D:                                                                   break;      //Fremad
  case 0xFF22DD: PosBase=PosBase+DegreeToTurnWithIR; slowmotor(PosBase,1, 10);     break;      //Vendstre
  case 0xFF02FD: PosBase=PosArm1=PosArm2=PosArm3=PosGrib=90;   
  slowmotor(PosBase,1, 10); slowmotor(PosArm1,2, 10); slowmotor(PosArm2,3, 10); 
  slowmotor(PosArm3,4, 10); slowmotor(PosGrib, 5, 10);                             break;      //Ok         
  case 0xFFC23D: PosBase=PosBase-DegreeToTurnWithIR; slowmotor(PosBase,1, 10);     break;      //Højre
  case 0xFFA857:                                                                   break;      //Tilbage
  case 0xFF6897: PosArm1=PosArm1+DegreeToTurnWithIR; slowmotor(PosArm1,2, 10);     break;      //1
  case 0xFF9867: PosArm1=PosArm1-DegreeToTurnWithIR; slowmotor(PosArm1,2, 10);     break;      //2
  case 0xFFB04F:                                                                   break;      //3
  case 0xFF30CF: PosArm2=PosArm2-DegreeToTurnWithIR; slowmotor(PosArm2,3, 10);     break;      //4
  case 0xFF18E7: PosArm2=PosArm2+DegreeToTurnWithIR; slowmotor(PosArm2,3, 10);     break;      //5
  case 0xFF7A85:                                                                   break;      //6
  case 0xFF10EF: PosArm3=PosArm3+DegreeToTurnWithIR; slowmotor(PosArm3,4, 10);     break;      //7
  case 0xFF38C7: PosArm3=PosArm3-DegreeToTurnWithIR; slowmotor(PosArm3,4, 10);     break;      //8
  case 0xFF5AA5:                                                                   break;      //9
  case 0xFF42BD: PosGrib=PosGrib+DegreeToTurnWithIR; slowmotor(PosGrib,5, 10);     break;      //*
  case 0xFF4AB5: PosGrib=PosGrib-DegreeToTurnWithIR; slowmotor(PosGrib,5, 10);     break;      //0
  case 0xFF52AD:                                                                   break;      //#
  case 0xFFFFFFFF:                                                                 break;      //Gentag
  default: 
  break;
  }
  delay(delayWithIR); 
} 

void slowmotor(int turnSlut, int motor, char delaymotor)
{
  int turnStart;
  switch(motor)
  {
  case 1:                                           // Base servomotor
  turnStart = ServoBase.read();
  if (turnSlut < turnStart)
  {
    for (int i = turnStart; i > turnSlut; i-- )
    {
      ServoBase.write(i);
      delay(delaymotor);
    }
  }
    else
  {
    for (int i = turnStart; i < turnSlut; i++ )
    {
      ServoBase.write(i);
      delay(delaymotor);
    }
  }
  break;
  
  case 2:                                       //ServoArm1
  turnStart = ServoArm1.read();
  if (turnSlut < turnStart)
  {
    for (int i = turnStart; i > turnSlut; i-- )
    {
      ServoArm1.write(i);
      delay(delaymotor);
    }
  }
    else
  {
    for (int i = turnStart; i < turnSlut; i++ )
    {
      ServoArm1.write(i);
      delay(delaymotor);
    }
  }
  break;

  case 3:                                      //ServoArm2
  turnStart = ServoArm2.read();
  if (turnSlut < turnStart)
  {
    for (int i = turnStart; i > turnSlut; i-- )
    {
      ServoArm2.write(i);
      delay(delaymotor);
    }
  }
    else
  {
    for (int i = turnStart; i < turnSlut; i++ )
    {
      ServoArm2.write(i);
      delay(delaymotor);
    }
  }
  break;

  case 4:                                     //ServoArm3
  turnStart = ServoArm3.read();
  if (turnSlut < turnStart)
  {
    for (int i = turnStart; i > turnSlut; i-- )
    {
      ServoArm3.write(i);
      delay(delaymotor);
    }
  }
    else
  {
    for (int i = turnStart; i < turnSlut; i++ )
    {
      ServoArm3.write(i);
      delay(delaymotor);
    }
  }
  break;

  case 5:                                    //ServoGrib
  turnStart = ServoGrib.read();
  if (turnSlut < turnStart)
  {
    for (int i = turnStart; i > turnSlut; i-- )
    {
      ServoGrib.write(i);
      delay(delaymotor);
    }
  }
    else
  {
    for (int i = turnStart; i < turnSlut; i++ )
    {
      ServoGrib.write(i);
      delay(delaymotor);
    }
  }
  break;

 default: 
  break;

  }
}

