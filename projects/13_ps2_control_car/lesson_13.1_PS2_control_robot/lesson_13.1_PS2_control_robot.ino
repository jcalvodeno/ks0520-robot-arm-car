/*
 keyestudio 4DOF Mechanical Robot Arm Car
 lesson 13.1
 PS2 control robot
 http://www.keyestudio.com
*/ 

#include <PS2X_lib.h>
#define PS2_DAT   12
#define PS2_CMD   11
#define PS2_SEL   10
#define PS2_CLK   13
int AIN2=2;  //define the driving pins of motor
int PWMA=3; 
int BIN2=4; //when AIN2 is low and AIN1 is high, BIN2 is high and BIN1 is low
int PWMB=5;
#define pressures   false
#define rumble    false
PS2X ps2x;
int error=0;
byte type=0;
byte vibrate=0;

void setup(){
  Serial.begin(9600);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  delay(1000);  

  for(int i = 0; i < 5; i++) {
    error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
    
    if(error == 0) {
      type = ps2x.readType();

      switch(type) {
        case 0: Serial.println("unknown"); break;
        case 1: Serial.println("DualShock"); break;
        case 2: Serial.println("GuitarHero"); break;
        case 3: Serial.println("wireless controller"); break;
      }
      break;
    } else {
      Serial.print("Initialization failed. ");
      Serial.println(error);
      Serial.println("1 = No connection, 2 = No response, 3 = Mode rejection");
      delay(1000);  
    }
  }
  
  stopp();
}

void loop(){
  if(error != 0) {
    static unsigned long lastTry = 0;
    if(millis() - lastTry > 5000) {
      error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
      if(error == 0) {
        Serial.println("The handle has been successfully reconnected！");
      }
      lastTry = millis();
    }
    delay(1000);
    return;
  }
  

    ps2x.read_gamepad(false, vibrate);

    vibrate = ps2x.Analog(PSAB_CROSS);
    if (ps2x.NewButtonState()){
      if(ps2x.Button(PSB_L2))       {Serial.println("L2 pressed,stop");stopp();}
      if(ps2x.Button(PSB_R2))       {Serial.println("R2 pressed,stop");stopp();}
      if(ps2x.Button(PSB_GREEN))    {Serial.println("Triangle pressed,advance");advance();}
      if(ps2x.Button(PSB_RED))      {Serial.println("Circle pressed,turn right");turnR();}
      if(ps2x.Button(PSB_BLUE))     {Serial.println("X pressed,back");back();}
      if(ps2x.Button(PSB_PINK))     {Serial.println("Square pressed,turn left");turnL();}
    }     delay(50); 
  }

void advance() {   //go forward
  digitalWrite(AIN2, LOW); //when AIN2 is low and AIN1 is high，motor MA turns clockwise
  analogWrite(PWMA, 100); //rotation speed of motor MA is 100
  digitalWrite(BIN2, HIGH); //When BIN2 is high and BIN1 is low, motor MB turns clockwise
  analogWrite(PWMB, 100); //rotation speed of motor MB is 100
}
void turnL() {  //turn left
  digitalWrite(AIN2, HIGH); //when AIN2 is high and AIN1 is low，motor MA turns anticlockwise
  analogWrite(PWMA, 100); //rotation speed of motor MA is 100
  digitalWrite(BIN2, HIGH); //When BIN2 is high and BIN1 is low, motor MB turns clockwise
  analogWrite(PWMB, 100); //rotation speed of motor MB is 100
}
void turnR() {  //turn right
  digitalWrite(AIN2, LOW); //when AIN2 is low and AIN1 is high，motor MA turns clockwise
  analogWrite(PWMA, 100); //rotation speed of motor MA is 100
  digitalWrite(BIN2, LOW); //When BIN2 is low and BIN1 is high, motor MB turns clockwise
  analogWrite(PWMB, 100); //rotation speed of motor MB is 100
}
void back() {   //go back
  digitalWrite(BIN2, LOW); //When BIN2 is low and BIN1 is high motor MB turns anticlockwise
  analogWrite(PWMB, 100); //rotation speed of motor MB is 100
  digitalWrite(AIN2, HIGH); //when AIN2 is high and AIN1 is low，motor MA turns anticlockwise
  analogWrite(PWMA, 100); //rotation speed of motor MA is 100
}
void stopp() {  //stop
  analogWrite(PWMA, 0); //rotation speed of motor MA is 0
  analogWrite(PWMB, 0); //rotation speed of motor MB is 0
}
