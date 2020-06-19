#include <Servo.h> 
#include "IRremote.h"
#include <string.h>

volatile char state;
enum state {initialisering, StartBesked, InitIR, StyringMedIR, StyringMedKode, StyringMedSerial, StyringMedGamepad};

int receiver = 8;
int ServoBasePin = 9;
int ServoArm1Pin = 10;
int ServoArm2Pin = 11;
int ServoArm3Pin = 12;
int ServoGribPin = 13;

int PosBase=90;             //Start betingelser ved IR 
int PosArm1=90;
int PosArm2=90; 
int PosArm3=90;
int PosGrib=90;
int DegreeToTurnWithIR, delayWithIR, Valg;

int Positionvalg;
int Motorvalg;

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

Servo ServoBase, ServoArm1, ServoArm2, ServoArm3, ServoGrib;    

IRrecv irrecv(receiver);     // Biblioteket søger for gemme resultatet i "results". Den retunere 0 hvis der er ingen data. Den retunere 1 hvis dataen er klar
decode_results results;      //...

void setup() 
{ 
  state = initialisering;
  inputString.reserve(200);
} 
 
void loop()
{
    switch(state) {
        case initialisering:{
              Serial.begin(9600);
              ServoBase.attach(ServoBasePin); 
              ServoArm1.attach(ServoArm1Pin); 
              ServoArm2.attach(ServoArm2Pin); 
              ServoArm3.attach(ServoArm3Pin); 
              ServoGrib.attach(ServoGribPin);
              state = StartBesked; 
        }break;

        case StartBesked:{
            Serial.println("Hvordan oensker du at styre robotarmen?");
            Serial.println("1: Robotarm styret med kode skrevet paa forhaand");
            Serial.println("2: Robotarm styret af fjernbetrjening");
            Serial.println("3: Robotarm styret af serial kommunikation med denne PC");
            Serial.println("4: Robotarm styret af serial kommunikation med denne PC og en spilcontroller (husk at koere python scriptet!)");
            Serial.println("Ved at angive tallet '1000' starter programmet forfra");
            while (Serial.available() == 0);
            Valg = Serial.parseInt();
            if (Valg==1){ 
              Serial.println();
              Serial.println("Velkommen til styring af robotarm med kode skrevet på forhaand! Robotarmen vil begynde nu");
              state=StyringMedKode;}
             else if (Valg==2){
              state=InitIR;}
             else if (Valg==3){
              Serial.println();
              Serial.println("Velkommen til styring af robotarmen med PC! Positionen vaelges i grader og ved motoren gaelder:");
              Serial.println("Base=1, Armen taettest paa basen=2, Armen i midten=3, Yderste arm=4, Gribeklo=5. Ved kloen sættes til position 125 er kloen lukket imens den ved 50 er aaben");
              state=StyringMedSerial;}
             else if (Valg==4) {
              Serial.println();
              Serial.println("Velkommen til styring af robotarmen med spilcontroller! Kommandoer fra PC afventes...");
              state=StyringMedGamepad;}     
            else {
              Serial.println("Ingen af valgmulighederne blev valgt. Programmet genstarter");
              state=initialisering;};
        }break;

        case InitIR:{
              Serial.println();
              Serial.write("Velkommen til styring af robotarm med fjernbetjening. Vaelg venligst foelsomheden (Grader pr tryk):");   //typisk 2-7
              while (Serial.available() == 0);
              DegreeToTurnWithIR = Serial.parseInt();
              if (DegreeToTurnWithIR==1000){
                  state=initialisering;}
              Serial.write(" - ");
              Serial.println(DegreeToTurnWithIR);
              Serial.write("Vaelg min hastighed mellem hvert tryk i ms:");     //Typsik 20-300
              while (Serial.available() == 0); 
              delayWithIR = Serial.parseInt();
              if (delayWithIR==1000){
                  state=initialisering;}
              Serial.write(" - ");
              Serial.println(delayWithIR);
              Serial.println("Fjernbetjeningen kan nu benyttes!");
              Serial.println("Information omkring styring af fjernbetjening:");
              Serial.println("Vnstre + Højre styrer basen");
              Serial.println("1 + 2 styrer den nederste del af armen");
              Serial.println("4 + 5 styrer den midterste del af armen");
              Serial.println("7 + 8 styrer den øverste del af armen");
              Serial.println("* + 0 styrer kloen");
              Serial.println("'OK' reseter armens bevægelser og sætter den tilbage til startposition");
              state=StyringMedIR;
        }break;

        case StyringMedIR:{
              if (irrecv.decode(&results)){
                   translateIR(); 
                   irrecv.resume();}
                   
              if (Serial.available() > 0){
                if (Serial.parseInt()==1000){
                    state=initialisering;}}
              state=StyringMedIR;
        }break;

        case StyringMedKode:{
             slowmotor(125,5, 5);       
             delay(500);   
             slowmotor(50,5, 5);      
             delay(500);   
             slowmotor(0,4, 5);      
             delay(500);   
             slowmotor(90,4, 5);   
             delay(500);   
             slowmotor(180,4, 5);   
             
             if (Serial.available() > 0){
                if (Serial.parseInt()==1000){
                    state=initialisering;}}
             state=StyringMedKode;
        }break;

        case StyringMedSerial:{
            Serial.write("Hvilken position?");
            while (Serial.available() == 0);
            Positionvalg = Serial.parseInt();
            if (Positionvalg==1000){
               state=initialisering;}
            Serial.write(" - ");
            Serial.println(Positionvalg);
          
            Serial.write("Hvilken motor?");
            while (Serial.available() == 0);
            Motorvalg = Serial.parseInt();
            if (Motorvalg==1000){
               state=initialisering;}
            Serial.write(" - ");
            Serial.println(Motorvalg);
            
            if ((Positionvalg < 181) && (Motorvalg < 6)){
              slowmotor(Positionvalg, Motorvalg, 10);}
            state=StyringMedSerial;
        } break;

        case StyringMedGamepad:{
            
            //Modtagelse af kommandoen fra PC
            //Serial.print('.');
            //while(Serial.available() == 0);

            //Splitte kommandoen de steder, hvor der er ';' 
            if (stringComplete) {
              Serial.println(inputString);
              // clear the string:
              inputString = "";
              stringComplete = false;
            }
            
            if ((Positionvalg < 181) && (Motorvalg < 6)){
              slowmotor(Positionvalg, Motorvalg, 10);}
            state=StyringMedGamepad;
        } break;
        
        default:{
          Serial.println();
          Serial.println("Et problem med programmet opstod. Programmet genstarter...");
          state=initialisering;
        }break;
    }
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    Serial.println("test");
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
      Serial.println(inputString);      
      Serial.println(inChar);      

    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      Serial.println("OK");
    }
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


/*
 *  Bliver kaldet med parameterne (Drejegrad, motor, hastighed)
 *  Drejegrad er alt imellem 0 og 180
 *  Ved motor gælder følgende: Base=1, Nederste arm=2, Midterste arm=3, Øverste arm=4, Gribeklo=5
 *  Hastighed: 0=hurtig, 30=langsomt
 *  Ved gribekloen gælder at:  grib=125 (lukket) 50(Åben) 
 */
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

