#include "Arduino.h" //!This library is needed to define the pins used to control the arduino
#include "Servo.h"

//-----------------Begin Definitions Section--------------------//
//? The section of coding below redefines the arduino pins to more user frinendly names

//Sensors
#define RSen A2   //Right edge or line sensor (pin 22)
#define LSen A1   //Left edge or line sensor (pin 21)
#define CSen A0   //Center line sensor (pin 20)
#define Sharp A3  //Targeting Sensor (pin 23)
#define AssSensor 12

//Motor control
#define ENR 6   //Right motor PWM (pin 9)
#define RD1 2   //Right motor dir1 (pin 5)
#define RD2 4   //Right motor dir2 (pin 7)
#define ENL 5   //Left motor PWM (pin 8)
#define LD1 7   //Left motor dir1 (pin 10)
#define LD2 8   //Left motor dir2 (pin 11)
//Outputs
#define Led 13  //Onboard led for diag

//Switches
//?The definitions below are switches to Enable and Disable secions of predefined logic
// Remove the "//" before a definition to Enable or add to Disable
// #define Debug       //? Enables serial printout of sensors
//#define RampEn      //? Enables motor ramp up logic !!not finished!!
 
//------------------End Definitions Section---------------------//

//-------------Begin Variable Definitions Section---------------//
//User adjustable program variables
int EdgeLVL = 50;   //Edge detection threshold
int SharpLvl = 250;  //Threshold for sharp targeting sensor (0-1023 higher number shortens detection range)
int MinSpd = 130;   //Minimum speed for motors to run (0-255  default: 0)
int FwdSpd = 130;   //Variable for reverse speed (0-255  default: 150)
int RevSpd = 190;   //Variable for reverse speed (0-255  default: 100)
int Turn = 255;     //Variable for turnig speed (0-255  default: 80)
int RSenOff = 0;    //Right senosr offest to compensate for bad alignment
int LSenOff = 0;    //Left sensor offest to compensate for bad alignment
int MaxSpd = 200;   //Maximum speed for all motions (0-255  default: 200)
int Codeloop = 0;
//Program variables //!MODIFICATION NOT RECOMENDED!!
int LCur;   // Variable to store left sensor current value
int RCur;   // Variable to store right sensor current value
int LThr;
int RThr;
int CurSpd = 0; // Variable to store current speed threshold
float Ramp; // Variable to store ramp up value for fwd motion

//--------------End Variable Definitions Section----------------//

//----------Begin Routine & Logic Definitions Section-----------//


void Fwd(){
  analogWrite(ENR,CurSpd);  //right motor PWM signal
  digitalWrite(RD1,HIGH);   //right motor sw1
  digitalWrite(RD2,LOW);    //right motor sw2
  analogWrite(ENL,CurSpd);  //left motor PWM signal
  digitalWrite(LD1,LOW);   //left motor sw1
  digitalWrite(LD2,HIGH);    //left motor sw2
  delay(1); 
}

void Brk(){
  analogWrite(ENR,HIGH);    //set right PWM on
  digitalWrite(RD1,HIGH);   //right motor sw1
  digitalWrite(RD2,HIGH);   //right motor sw2
  analogWrite(ENL,HIGH);    //set left PWM on
  digitalWrite(LD1,HIGH);   //left motor sw1
  digitalWrite(LD2,HIGH);   //left motor sw2
  delay(1);
}

void RotR(){
  analogWrite(ENR,(0+Turn));  //set right PWM on
  digitalWrite(RD1,HIGH);      //right motor sw1
  digitalWrite(RD2,LOW);     //right motor sw2
  analogWrite(ENL,(0-Turn));  //set left PWM on
  digitalWrite(LD1,HIGH);     //left motor sw1
  digitalWrite(LD2,LOW);      //left motor sw2
  delay(1);
}

void RotL(){
  analogWrite(ENR,(0-Turn));  //set right PWM on
  digitalWrite(RD1,LOW);     //right motor sw1
  digitalWrite(RD2,HIGH);      //right motor sw2
  analogWrite(ENL,(0+Turn));  //set left PWM on
  digitalWrite(LD1,LOW);      //left motor sw1
  digitalWrite(LD2,HIGH);     //left motor sw2
  delay(1);
}

void Rev(){
  analogWrite(ENR,RevSpd);  //right motor PWM signal
  digitalWrite(RD1,LOW);   //right motor sw1
  digitalWrite(RD2,HIGH);    //right motor sw2
  analogWrite(ENL,RevSpd);  //left motor PWM signal
  digitalWrite(LD1,HIGH);   //left motor sw1
  digitalWrite(LD2,LOW);    //left motor sw2
  delay(1); 
}


//-----------End Routine & Logic Definitions Section------------//

//-----------------Setup Logic (runs once)----------------------//

void setup() {
  //set pinmodes based on device type
  pinMode(RSen,INPUT);
  pinMode(LSen,INPUT);
  pinMode(CSen,INPUT);
  pinMode(Sharp,INPUT);
  pinMode(ENR,OUTPUT);
  pinMode(RD1,OUTPUT);
  pinMode(RD2,OUTPUT);
  pinMode(ENL,OUTPUT);
  pinMode(LD1,OUTPUT);
  pinMode(LD2,OUTPUT);
  pinMode(Led,OUTPUT);
  pinMode(AssSensor, INPUT_PULLUP); 


   LThr = EdgeLVL;
   RThr = EdgeLVL;

  #ifdef Debug
    Serial.begin(19200);
  #endif

  #ifndef RampEn
    CurSpd = FwdSpd;
  #endif

  //5 second delay at the start of the match //!DO NOT REMOVE for Sumo battles//
  for(int i=0; i<=20; i++){
    digitalWrite(Led,HIGH);
    delay(125);
    digitalWrite(Led,LOW);
    delay(125);
  }
  

  
  
                                                              //
  //**************************************************************************//
}

//-------------END Setup Logic (runs once)----------------------//

//-------------Main Logic (runs continuous)---------------------//

//MAIN Loop

void loop() { 
  int LCur = (analogRead(LSen)+LSenOff);  //read current value for sensor into LCur with offset of Left sensor
  int RCur = (analogRead(RSen)+RSenOff);  //read current value for sensor into Rcur with offset of Right sensor
  int SharpCur = analogRead(Sharp);
  int AssValue = digitalRead(AssSensor);

  #ifdef Debug
    Serial.print("left sensor: ");
    Serial.println(LCur,DEC);
    Serial.print("Right sensor: ");
    Serial.println(RCur);
    Serial.print("Sharp sensor: ");
    Serial.println(SharpCur); 
    Serial.print("ASS sensor: ");
    Serial.println(AssValue);    
    delay(1);
  #endif


  if (RCur < RThr){                //Line detected on the right, stop..reverse..turn left..
    Ramp = 0;                         //Reset ramp to 0
    Brk();
    delay(500);
    Rev();
    delay(800);
    RotR();
    delay(800);  
  } else if (LCur < LThr){         //Line detected on the right, stop..reverse..turn right..
    Ramp = 0;                         //Reset ramp to 0
    Brk();
    delay(500);
    Rev();
    delay(800);
    RotR();
    delay(800);
  } else if (SharpCur > SharpLvl){
    Fwd();
  }else if (AssValue == LOW){
    digitalWrite(LED_BUILTIN, HIGH);
    RotR();
    delay(2000);
    Fwd();
    delay(5000);  
  } else{
    RotR();
  }
}