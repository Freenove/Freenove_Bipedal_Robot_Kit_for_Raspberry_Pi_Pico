/**********************************************************************
  Product     : Freenove Robot for Raspberry Pi Pico (W)
  Description : Control the robot movement.
  Auther      : www.freenove.com
  Modification: 2023/11/07
**********************************************************************/
//Check that the servos are properly connected to the corresponding interface before uploading the code
/************************************
           ---     ---
         --------------- 
        |     O   O     |
        |---------------|
YR 12==>|               | <== YL 10
         --------------- 
            ||     ||
            ||     ||
RR 13==>  -----   ------  <== RL 11
         |-----   ------|
************************************/
#include <Arduino.h>
#include <SoftwareSerial.h>  
#include <EEPROM.h>
#include "Freenove_Robot_For_Pico_W.h"

#include "SerialCommand.h"
SoftwareSerial BTserial = SoftwareSerial(1, 0);  // RX pin to 1 and TX pin to 0 on the board
SerialCommand SCmd(BTserial);
#include "Bipedal_Robot.h"
Bipedal_Robot Bipedal_Robot;

#define LeftLeg 10
#define RightLeg 12
#define LeftFoot 11
#define RightFoot 13

int YL;
int YR;
int RL;
int RR;

int T = 1000;
int moveId = 0;
bool isServoResting = true;

extern int moveId;
extern bool isServoResting;

int moveSize = 15;

void receiveStop() {
  sendAck();
  sendFinalAck();
}

void recieveBuzzer() {
  sendAck();
  bool error = false;
  int frec;
  int duration;
  char *arg;
  arg = SCmd.next();
  if (arg != NULL) frec = atoi(arg);
  else error = true;
  Serial.println("\nfrec is:");
  Serial.println(frec);
  tone(20, frec);
  sendFinalAck();
}

void receiveMovement() {
  sendAck();
  Serial.print("move loop...");
  if (Bipedal_Robot.getRestState() == true) Bipedal_Robot.setRestState(false);
  isServoResting = Bipedal_Robot.getRestState();
  char *arg;
  arg = SCmd.next();
  if (arg != NULL) {
    moveId = atoi(arg);
    Serial.print("moveId:");
    Serial.print(moveId);
  } else {
    moveId = 0;
  }
  arg = SCmd.next();
  if (arg != NULL) {
    T = atoi(arg);
    if (T == 0) {
      T = 1000;
    } else if (T == 1) {
      T = 1500;
    } else if (T == 2) {
      T = 2000;
    }
  } else T = 2000;
  Serial.print("T is:");
  Serial.print(T);
  arg = SCmd.next();
  if (arg != NULL) moveSize = atoi(arg);
  else moveSize = 15;
}
void move(int moveId) {
  // bool manualMode = false;
  switch (moveId) {
    case 0:
      Bipedal_Robot.home();
      // Serial.print("moveId 0 home");
      break;
    case 1:
      Bipedal_Robot.walk(1, T, 1);
      break;
    case 2:
      Bipedal_Robot.walk(1, T, -1);
      break;
    case 3:
      Bipedal_Robot.turn(1, T, 1);
      break;
    case 4:
      Bipedal_Robot.turn(1, T, -1);
      break;
    default:
      break;
  }
}

void sendAck() {
  delay(10);
  Serial.flush();
}
void sendFinalAck() {
  delay(10);
  Serial.flush();
}

void setup() {
  Serial.begin(9600);
  BTserial.begin(9600);
  //初始化一些传感器
  Buzzer_Setup();  //Initialize the buzzer
  EEPROM.begin(512);
  Bipedal_Robot.init(LeftLeg, RightLeg, LeftFoot, RightFoot, true);  //Set the servo pins and Buzzer pin

  YL = EEPROM.read(0);
  if (YL > 128) YL -= 256;
  YR = EEPROM.read(1);
  if (YR > 128) YR -= 256;
  RL = EEPROM.read(2);
  if (RL > 128) RL -= 256;
  RR = EEPROM.read(3);
  if (RR > 128) RR -= 256;
  Bipedal_Robot.setTrims(YL, YR, RL, RR);
  calib_homePos();
  Bipedal_Robot.saveTrimsOnEEPROM();
  Bipedal_Robot.home();
  delay(100);
  EEPROM.commit();
  SCmd.addCommand("S", receiveStop);
  SCmd.addCommand("D", recieveBuzzer);
  SCmd.addCommand("A", receiveMovement);
  SCmd.addDefaultHandler(receiveStop);
  Bipedal_Robot.home();
}

void loop() {
  SCmd.readSerial();
}

void calib_homePos() {
  int servoPos[4];
  servoPos[0] = 90;
  servoPos[1] = 90;
  servoPos[2] = 90;
  servoPos[3] = 90;
  Bipedal_Robot._moveServos(500, servoPos);
  Bipedal_Robot.detachServos();
}

void setup1() {
}
void loop1() {
  move(moveId);
}
