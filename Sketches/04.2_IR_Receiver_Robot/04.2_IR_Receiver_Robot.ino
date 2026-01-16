/**********************************************************************
  Filename    : IR_Receiver_Car.ino
  Product     : Freenove Robot for Raspberry Pi Pico (W)
  Auther      : www.freenove.com
  Modification: 2023/11/07
**********************************************************************/
#include <Arduino.h>
#include <IRremote.hpp>
#include <EEPROM.h>
#include "Bipedal_Robot.h"

#define LeftLeg 10
#define RightLeg 12
#define LeftFoot 11
#define RightFoot 13

#define IR_Pin 3
#define ENABLE_LED_FEEDBACK true
#define DISABLE_LED_FEEDBACK false

// #define USE_NO_SEND_PWM

int move_flag = 0;

Bipedal_Robot Bipedal_Robot;
//int calibratePosition[4] = { -17, -10, -15, 12 };

void calib_homePos() {
  int servoPos[4];
  servoPos[0] = 90;
  servoPos[1] = 90;
  servoPos[2] = 90;
  servoPos[3] = 90;
  Bipedal_Robot._moveServos(500, servoPos);
  Bipedal_Robot.detachServos();
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  Bipedal_Robot.init(LeftLeg, RightLeg, LeftFoot, RightFoot, true);  //Set the servo pins 
  //Bipedal_Robot.setTrims(calibratePosition[0], calibratePosition[1], calibratePosition[2], calibratePosition[3]);
  calib_homePos();
  Bipedal_Robot.saveTrimsOnEEPROM();
  EEPROM.commit();
  Bipedal_Robot.home();
  IrReceiver.begin(IR_Pin, DISABLE_LED_FEEDBACK);  // Start the receiver
}

void loop() {
  if (IrReceiver.decode()) {
    unsigned long value = IrReceiver.decodedIRData.decodedRawData;
    handleControl(value);        // Handle the commands from remote control
    Serial.println(value, HEX);  // Print "old" raw data
    Serial.println();
    IrReceiver.resume();  // Enable receiving of the next value
  }
  Move(move_flag);
}

void handleControl(unsigned long value) {
  // Handle the commands
  switch (value) {
    case 0xBF40FF00:  // Receive the number '+'
      move_flag = 1;
      break;
    case 0xE619FF00:  // Receive the number '-'
      move_flag = 2;
      break;
    case 0xF807FF00:  // Receive the number '|<<'
      move_flag = 3;
      break;
    case 0xF609FF00:  // Receive the number '>>|'
      move_flag = 4;
      break;
    case 0xEA15FF00:  // Receive the number '▶'
      move_flag = 0;
      break;
    default:
      move_flag = 0;
      break;
  }
}
void Move(int data) {
  // Handle the commands
  switch (data) {
    case 1:  // Receive the number '+'
      Bipedal_Robot.walk(2, 1500, 1);
      break;
    case 2:  // Receive the number '-'
      Bipedal_Robot.walk(2, 1500, -1);
      break;
    case 3:  // Receive the number '|<<'
      Bipedal_Robot.turn(2, 1500, 1);
      break;
    case 4:  // Receive the number '>>|'
      Bipedal_Robot.turn(2, 1500, -1);
      break;
    case 0:  // Receive the number '▶'
      Bipedal_Robot.home();
      break;
    default:
      move_flag = 0;
      Bipedal_Robot.home();
      break;
  }
}
