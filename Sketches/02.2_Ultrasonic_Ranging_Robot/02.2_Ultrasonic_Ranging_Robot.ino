/**********************************************************************
  Product     : Freenove Robot for Raspberry Pi Pico (W)
  Description : Ultrasonic Robot.
  Auther      : www.freenove.com
  Modification: 2023/11/07
**********************************************************************/
/*Check that the servos are properly connected to the corresponding 
interface before uploading the code. */
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
#include "Freenove_Robot_For_Pico_W.h"
#include "Bipedal_Robot.h"
Bipedal_Robot Bipedal_Robot;

#define LeftLeg 10
#define RightLeg 12
#define LeftFoot 11
#define RightFoot 13


void setup() {
  Serial.begin(115200);
  Ultrasonic_Setup();           //Initialize the ultrasonic module
  Bipedal_Robot.init(LeftLeg, RightLeg, LeftFoot, RightFoot, true); //Set the servo pins
  Bipedal_Robot.home();
  delay(50);
}
void loop()
{
  if (Get_Sonar() <= 20)
  {
    Bipedal_Robot.sing(S_surprise);
    Bipedal_Robot.walk(2, 1000, -1); // BACKWARD x2
    Bipedal_Robot.turn(3, 1000, 1); // LEFT x3
  }
  Bipedal_Robot.walk(1, 1200, 1); // FORWARD x1
}
