#include "AccelStepper.h"
//26680 old competition steps
//13340 for new lead screw - quarterstep
//6670 for twice
//1667 for full steps


int leadStep=13;
int leadDir=12;

int slavePinLEAD=2;

int enableLead=7;

AccelStepper leadMotor(AccelStepper::DRIVER, leadStep,leadDir);

int LEADms1=11;
int LEADms2=10;
int LEADms3=9;
int maxSpeed=3000; 
int maxAccel=3000; 

void setup() {
  pinMode(enableLead,OUTPUT);
  digitalWrite(enableLead,HIGH);
  
  pinMode(slavePinLEAD,INPUT);
  digitalWrite(slavePinLEAD,LOW);
  
  pinMode(LEADms1,OUTPUT);
  pinMode(LEADms2,OUTPUT);
  pinMode(LEADms3,OUTPUT);
  digitalWrite(LEADms1,LOW);
  digitalWrite(LEADms2,HIGH);
  digitalWrite(LEADms3,LOW);

  leadMotor.setMaxSpeed(maxSpeed);
  leadMotor.setAcceleration(maxAccel);

//TESTING
//  digitalWrite(enableLead,LOW);
//  leadMotor.move(6670); 
//      while (leadMotor.distanceToGo()!=0)
//     {
//       leadMotor.run();
//     }

}
//UNCOMMENT THIS
  void loop() { 
    if(digitalRead(slavePinLEAD)==HIGH)
    {
      delay(3000);
      digitalWrite(enableLead,LOW);
      leadMotor.move(13400); 
      while (leadMotor.distanceToGo()!=0)
     {
       leadMotor.run();
     }
    }
    digitalWrite(enableLead,HIGH);
}
