/**********************************************************************
  Product     : Freenove Robot for Raspberry Pi Pico (W)
  Description : Use buzzer
  Auther      : www.freenove.com
  Modification: 2023/11/07
**********************************************************************/
#include "Freenove_Robot_For_Pico_W.h"

void setup() {
  Buzzer_Setup();      //Buzzer initialization function
  Buzzer_Alert(4, 3);  //Control the buzzer to sound 3 times, 4 sounds each time
}

void loop() {
  delay(1000);
}
