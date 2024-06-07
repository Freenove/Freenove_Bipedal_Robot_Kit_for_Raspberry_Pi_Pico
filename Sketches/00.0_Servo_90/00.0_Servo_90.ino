/**********************************************************************
  Product     : Freenove Robot for Raspberry Pi Pico (W)
  Description : The four servos were rotated to 90°.
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
#include "Bipedal_Robot.h"
Bipedal_Robot Bipedal_Robot;

#define LeftLeg 10
#define RightLeg 12
#define LeftFoot 11
#define RightFoot 13

void setup() {
  Serial.begin(115200);
  Bipedal_Robot.init(LeftLeg, RightLeg, LeftFoot, RightFoot, false);  //Set the servo pins and Buzzer pin
  Bipedal_Robot.home();
  delay(50);
}

void loop() {
  Serial.println("loop.....");
  int homes[4] = {90, 90, 90, 90}; //All the servos at rest position
  Bipedal_Robot._moveServos(500, homes);  //Move the servos in half a second
  delay(1000);
}
