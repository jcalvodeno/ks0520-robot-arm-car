/*
 keyestudio 4DOF Mechanical Robot Arm Car
lesson 15.1
 Bluetooth control robotic arm
 http://www.keyestudio.com
*/
#include <Servo.h>  //add the library of servo
Servo myservo1; //define the name of servo variable
Servo myservo2; //define the name of servo variable
Servo myservo3; //define the name of servo variable
Servo myservo4; //define the name of servo variable
int pos1=90,pos2=100,pos3=80,pos4=90; // define angle variable of four servos(angle value of posture when starting up)
void T_left(){  //turn left
  pos1+=1;
  myservo1.write(pos1);
  delay(10);
  if(pos1>=180){  //set the limited angle value of servo
    pos1=180;
  }}
void T_right(){  //turn right
  pos1-=1;
  myservo1.write(pos1);
  delay(10);
  if(pos1<=0){  //unclamped per user request - wants full counterclockwise travel toward 8-9 o'clock
    pos1=0;
  }}
void ZB(){  //claw closes
  pos4-=1;
  myservo4.write(pos4);
  delay(5);
  if(pos4<=18){  //calibrated floor - measured fully-closed angle on this rebuild
    pos4=18;
  }}
void ZK(){  //claw opens
  pos4+=1;
  myservo4.write(pos4);
  delay(5);
  if(pos4>=180){
    pos4=180;
  }}
void LF(){  //smaller arm lifts up
  pos2+=1;
  myservo2.write(pos2);
  delay(10);
  if(pos2>=120){  //raised per user request - confirmed reachable
    pos2=120;
  }}
void LB(){  //smaller arm lifts down
  pos2-=1;
  myservo2.write(pos2);
  delay(10);
  if(pos2<=0){  //unclamped per user request
    pos2=0;
  }}
void RF(){  // bigger arm swings forward
  pos3+=1;
  myservo3.write(pos3);
  delay(10);
  if(pos3>=180){
    pos3=180;
  }}
void RB(){  // bigger arm swings back
  pos3-=1;
  myservo3.write(pos3);
  delay(10);
  if(pos3<=75){  //calibrated floor - conservative, this joint's real safe floor depends on servo2's position (not modeled here)
    pos3=75;
  }}
void setup(){
  Serial.begin(9600);
  myservo1.attach(A1);  //set control pin of servo 1 to A1
  myservo2.attach(A0);  //set control pin of servo 2 to A0
  myservo3.attach(8);   //set control pin of servo 3 to D8
  myservo4.attach(9);   //set control pin of servo 4 to D9
  myservo3.write(pos3);  //servo 3 rotates to 80° 
  delay(500);
  myservo2.write(pos2);  //servo 2 rotates to 100° 
  delay(500);
  myservo1.write(pos1);  //posture to start up, servo 1 rotates to 90°
  delay(500);
  myservo4.write(pos4);  //servo 4 rotates to 90° 
}
char activeCommand = 0; // which command is currently "held" - 0 means none

void loop(){
  if(Serial.available()>0){  //determine if Bluetooth receives signals
    char c = Serial.read();
    if(c=='s'){
      activeCommand = 0; //explicit stop signal from the app
    } else if(c=='Q'||c=='E'||c=='l'||c=='r'||c=='f'||c=='b'||c=='V'||c=='P'){
      activeCommand = c; //start/replace the held command
    }
    //any other byte (including noise) is ignored - does not change activeCommand
  }
  switch(activeCommand){
    case 'Q': LF(); break; //smaller arm lifts up
    case 'E': LB(); break; //smaller arm lifts down
    case 'l': T_left(); break; //mechanical arm turns left
    case 'r': T_right(); break; //mechanical arm turn right
    case 'f': RF(); break; //bigger arm swings forward
    case 'b': RB(); break; //bigger arm swings back
    case 'V': ZK(); break; //claw opens
    case 'P': ZB(); break; //claw closes
  }
  delay(5);
}
