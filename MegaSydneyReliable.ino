#include "AccelStepper.h"
#include <Servo.h>

long numberForRotation; 
int leftSTEP=29;
int leftDIR=28;
int rightSTEP=43; //purple
int rightDIR=42; //grey

//Enable motor pins
int enableLEFT=27;
int enableRIGHT=41;

//SLAVE PIN FOR LEADSCREW
int slavePinLEAD=2; 

//SERVOS
int servoPinGOLF=7;//150 closed - 50 open
int servoPinRACQUET=6;//30 OPEN - 110 CLOSED 
int servoPinBRIDGE=5;//0 DRAW BRIDGE ,  80 when retracted
Servo servoGOLF;
Servo servoRACQUET; 
Servo servoBRIDGE;

//FUNCTIONS FOR SERVOS
/*GOLF BALLS*/void deployGOLF(){
  servoGOLF.write(50);
}

/*BRIDGE*/void deployBRIDGE(){
  servoBRIDGE.write(0);  
  delay(500);
  servoRACQUET.write(30);
}


//Starting button
int switchInput=14;

//intialization values
const unsigned int maxSpeedMOVE=1000;//THIS SPEED AND ACCEL SHOULD WORK 100 PERCENT
const unsigned int maxAccelMOVE=1500;
const unsigned int maxSpeedROTATE=800;
const unsigned int maxAccelROTATE=800;
const unsigned int stepsPerRev=200;//FULL STEP
const unsigned int wheelDiameter=79;
const unsigned int distanceBetweenWheels=365;
int circumference=round(PI*wheelDiameter);
int stepz;

//QUARTER STEP
int LEFTms1=22;
int LEFTms2=24;
int LEFTms3=26; 
int RIGHTms1=36;
int RIGHTms2=38;
int RIGHTms3=40;


AccelStepper leftMotor(AccelStepper::DRIVER, leftSTEP,leftDIR);
AccelStepper rightMotor(AccelStepper::DRIVER,rightSTEP,rightDIR); 

//functions

void move(int distance)
{
  rightMotor.setMaxSpeed(maxSpeedMOVE);
  rightMotor.setAcceleration(maxAccelMOVE);
  leftMotor.setMaxSpeed(maxSpeedMOVE);
  leftMotor.setAcceleration(maxAccelMOVE);
  
  stepz = round( (float)distance / ( ( PI * (float)wheelDiameter ) / (float)stepsPerRev ) );
  rightMotor.move(stepz);
  leftMotor.move(-1*stepz);

  while (rightMotor.distanceToGo()!=0 || leftMotor.distanceToGo() != 0)
  {
    rightMotor.run();
    leftMotor.run();
  }
}

void rotateGradual(float distance, float turnRadius, int leftOrRight) // leftOrRight: left=-1, right=1
//Does not work with acceleration right now, constant speeds only.
//Example call in main code: rotateGradual(500, 318, 1);
//This should mean the robot turns right, along a circle of radius 318mm (31.8cm), going a distance of 500mm (50cm)
//The robot would end up facing 90 degrees to the right, going a quarter of the way around the virtual circle (if my maths is right)
{
  float innerDistance = distance; //Arc lengths, how much of the crcle drawn by the turnRadius should be travelled
  float outerDistance = distance + distanceBetweenWheels;
  float ratio = outerDistance / innerDistance; //Should be a bit more than 1, usually less than 2 unless distance < distanceBetweenWheels (<365), which is only for tight turns
  int forwardSteps = round( distance / ( ( PI * (float)wheelDiameter ) / (float)stepsPerRev ) ); //Steps to go only forward the given distance
  int setSpeed; //Used later, this is the constant speed the wheels run at
  
  if (leftOrRight==1) //If turning right
  {
    rightMotor.move(forwardSteps); //Right motor (inner motor) moves the specified distance
    leftMotor.move(-1 * forwardSteps * ratio); //Left motor (outer motor) moves a little extra

    while (rightMotor.distanceToGo()!=0 || leftMotor.distanceToGo()!=0)
    {
      setSpeed = maxSpeedMOVE;
      leftMotor.runSpeed(); //Left motor (outer motor) runs at normal speed
      setSpeed = maxSpeedMOVE * (1 - (ratio - floor(ratio))); //This will result in maxSpeed * (1 - a small amount), so a speed bit lower than maxSpeed
      rightMotor.runSpeed(); //Right motor (inner motor) runs slightly slower so that both finish their steps together
    }
  }
  else if (leftOrRight==-1) //If turning left. Using elseif so that the command must be passed with a 1 or -1 in the correct spot, to avoid messing up the argument positions
  {
    leftMotor.move(forwardSteps);
    rightMotor.move(forwardSteps * ratio);

    while (rightMotor.distanceToGo()!=0 || leftMotor.distanceToGo()!=0)
    {
      setSpeed = maxSpeedMOVE;
      rightMotor.runSpeed();
      setSpeed = maxSpeedMOVE * (1 - (ratio - floor(ratio)));
      leftMotor.runSpeed();
    }
  }
}

void rotate(double angle, int leftorright) // -1 = Right rotation, 1 = Left rotation
{ 
  rightMotor.setMaxSpeed(maxSpeedROTATE);
  rightMotor.setAcceleration(maxAccelROTATE);
  leftMotor.setMaxSpeed(maxSpeedROTATE);
  leftMotor.setAcceleration(maxAccelROTATE);
  
  numberForRotation = round( ( ( (float)1 / (float)2 ) * ((PI/(float)180)*float(angle)) * (float)distanceBetweenWheels ) / ( ( PI * (float)wheelDiameter ) / (float)stepsPerRev ) );
  rightMotor.move(leftorright*numberForRotation);
  leftMotor.move(leftorright*numberForRotation);
  
  while (rightMotor.distanceToGo()!=0 || leftMotor.distanceToGo() != 0)
  {
    rightMotor.run();
    leftMotor.run();
  }
  
}

//running functions
void setup()
{

//SERVO SETUP
  servoGOLF.attach(servoPinGOLF);
  servoRACQUET.attach(servoPinRACQUET);
  servoBRIDGE.attach(servoPinBRIDGE);            
  servoGOLF.write(150);
  servoRACQUET.write(130);
  servoBRIDGE.write(80);   

//enable pins for wheels
  pinMode(enableLEFT,OUTPUT);
  pinMode(enableRIGHT,OUTPUT);

//ENABLE PINS FOR MOTORS - LOW =ON , HIGH=OFF
  digitalWrite(enableLEFT, LOW);
  digitalWrite(enableRIGHT, LOW);
  
//SWITCH
  pinMode(switchInput,INPUT);
  digitalWrite(switchInput,HIGH);

//LEADSCREW
  pinMode(slavePinLEAD,OUTPUT);
  digitalWrite(slavePinLEAD,LOW);

//QUARTERSTEP
  pinMode(RIGHTms1,OUTPUT);
  pinMode(RIGHTms2,OUTPUT);
  pinMode(RIGHTms3,OUTPUT);
  pinMode(LEFTms1,OUTPUT);
  pinMode(LEFTms2,OUTPUT);
  pinMode(LEFTms3,OUTPUT);
  
  digitalWrite(RIGHTms1,LOW);
  digitalWrite(RIGHTms2,LOW);
  digitalWrite(RIGHTms3,LOW);
  
  digitalWrite(LEFTms1,LOW);
  digitalWrite(LEFTms2,LOW);
  digitalWrite(LEFTms3,LOW);

//SPEED
//  rightMotor.setMaxSpeed(maxSpeed);
//  rightMotor.setAcceleration(maxAccel);
//  leftMotor.setMaxSpeed(maxSpeed);
//  leftMotor.setAcceleration(maxAccel);

}

void loop()
{
    if (digitalRead(switchInput)==LOW)
  {
     delay(500);
     digitalWrite(enableLEFT, LOW);
     digitalWrite(enableRIGHT, LOW);
     
     digitalWrite(slavePinLEAD,HIGH);
     delay(50);
     digitalWrite(slavePinLEAD,LOW);
     
     move(500);     
     rotate(14,-1); //Was 12
     move(1800); //Was 1790
     
     rotate(103,1); //Was 103.5
     move(55); //Was 45
     deployGOLF();
     //delay(100000);
     delay(500);

     //digitalWrite(slavePinLEAD,HIGH);
     //delay(50);
     //digitalWrite(slavePinLEAD,LOW);     
     
     move(1530); //Was 1510
     deployBRIDGE();

//     rotateGradual(1000, 1500, 1);

     digitalWrite(enableLEFT, HIGH);
     digitalWrite(enableRIGHT, HIGH);

     delay(1000000000);
  }
}

