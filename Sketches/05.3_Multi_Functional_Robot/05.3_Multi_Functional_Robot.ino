/**********************************************************************
  Product     : Freenove Robot for Raspberry Pi Pico (W)
  Description : Multi Functional Robot.
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
#include <EEPROM.h>
#include "Bipedal_Robot.h"
#include "AudioFileSourceLittleFS.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"
#include "AudioFileSourceID3.h"
#include "AudioOutputI2S.h"
#include <SoftwareSerial.h>
#include "SerialCommand.h"
#include "Freenove_Robot_For_Pico_W.h"
#include "Freenove_Robot_Emotion.h"
#include "Freenove_Robot_WS2812.h"
#include "Freenove_VK16K33_Lib.h"

SoftwareSerial BTserial = SoftwareSerial(1, 0);  // RX pin to 0 and TX pin to 1 on the board
SerialCommand SCmd(BTserial);

Bipedal_Robot Bipedal_Robot;
AudioGeneratorMP3 *mp3;
AudioFileSourceLittleFS *file;
AudioOutputI2SNoDAC *out;
AudioFileSourceID3 *id3;

#define LeftLegPin 10
#define RightLegPin 12
#define LeftFootPin 11
#define RightFootPin 13
#define BuzzerPin 20

int motionFlag = 0;
int ultrasonicavoidMode = 0;
int playstartingmusic = 0;
int musicmode = 0;
int playsong = 0;
extern int playsong;  //Set the proportional coefficient
int T = 1000;
int moveId = 0;
int dance_steps = 0;
int Ultrasonic_Avoid_steps = 0;
bool isServoResting = true;
extern int moveId;
extern bool isServoResting;
int moveSize = 15;
#define ACTION_GET_VOLTAGE 'P'
#define ACTION_GET_DISTANCE 'E'
#define INTERVAL_CHAR '#'
unsigned long lastUploadVoltageTime;
unsigned long lastUploadDistanceTime;
#define UPLOAD_VOL_TIME 2000

int Resting = 0;
float Distance = 0;

// int calibratePosition[4] = { -17, -10, -15, 12 };
// int YL;
// int YR;
// int RL;
// int RR;

void upLoadVoltageToApp() {
  float voltage = 0;
  voltage = Get_Battery_Voltage();
  BTserial.print("P#");
  BTserial.println(int(voltage * 1000));
}

void upLoadDistanceToApp() {
  Distance = Get_Sonar();
  BTserial.print("E#");
  BTserial.println(Distance);
}

void receiveStop() {
  sendAck();
  sendFinalAck();
}

void recieveBuzzer() {
  sendAck();
  moveId = 0;
  playsong = 0;
  bool error = false;
  int frec;
  char *arg;
  arg = SCmd.next();
  if (arg != NULL) frec = atoi(arg);
  else error = true;
  tone(2, frec);
  sendFinalAck();
}

void receiveAvoid() {
  sendAck();
  ws2812_task_mode = 0;
  emotion_task_mode = 0;
  if (Bipedal_Robot.getRestState() == true) Bipedal_Robot.setRestState(false);
  isServoResting = Bipedal_Robot.getRestState();
  char *arg;
  arg = SCmd.next();
  if (arg != NULL) {
    ultrasonicavoidMode = atoi(arg);
    playsong = 0;
    if (ultrasonicavoidMode == 1) {
      moveId = 21;
      Ultrasonic_Avoid_steps = 0;
      playsong = 0;
    }
    if (ultrasonicavoidMode == 2) {
      dance_steps = 0;
      moveId = 22;
      playsong = 4;
    }
    if (ultrasonicavoidMode == 0) {
      moveId = 0;
      Resting = 1;
      playsong = 0;
    }
  }
  sendFinalAck();
}

void receiveEmotion() {
  sendAck();
  Emotion_Setup();
  moveId = 0;
  playsong = 0;
  char *arg;
  char *endstr;
  arg = SCmd.next();
  if (arg != NULL) {
    String stringOne = String(arg);  // converting a constant char into a String
    emotion_task_mode = stringOne.toInt();
    Emotion_SetMode(emotion_task_mode);  //显示静态表情
  } else {
  }
  sendFinalAck();
}

void receiveLED() {
  sendAck();
  moveId = 0;
  playsong = 0;
  char *arg;
  unsigned char paramters[3];
  char *endstr;
  arg = SCmd.next();
  if (arg != NULL) {
    String stringOne = String(arg);  // converting a constant char into a String
    ws2812_task_mode = stringOne.toInt();
  } else {
  }
  arg = SCmd.next();
  String stringOne1 = String(arg);
  paramters[0] = stringOne1.toInt();

  arg = SCmd.next();
  String stringOne2 = String(arg);
  paramters[1] = stringOne2.toInt();

  arg = SCmd.next();
  String stringOne3 = String(arg);
  paramters[2] = stringOne3.toInt();

  WS2812_Set_Color_1(15, paramters[0], paramters[1], paramters[2]);
  sendFinalAck();
}

void songmusic() {
  if (playsong > 0) {
    playmusic(1, playsong);
    if (playsong != 4) {
      playsong = 0;
    }
  } else {
    pinMode(6, OUTPUT);
    digitalWrite(6, LOW);
  }
}
void receiveMusic() {
  int music_song;
  sendAck();
  ws2812_task_mode = 0;
  emotion_task_mode = 0;
  if (Bipedal_Robot.getRestState() == true) Bipedal_Robot.setRestState(false);
  isServoResting = Bipedal_Robot.getRestState();
  char *arg;
  arg = SCmd.next();
  if (arg != NULL) {
    music_song = atoi(arg);
    playsong = music_song;
    playstartingmusic = music_song;
    clearEmtions();
    if (playsong == 1) {
      moveId = 18;  //play music  Hello.mp3
    } else if (playsong == 2) {
      moveId = 19;  //play music  Nicetomeetyou.mp3
    } else if (playsong == 3) {
      moveId = 20;  //play music  goodbye.mp3
      Serial.print("yundong");
    } else if (playsong == 0) {
      moveId = 0;
    }
    Serial.print("moveId:");
    Serial.print(moveId);
  } else {
    moveId = 0;
    Resting = 1;
  }
}

void playmusic(int playtimes, int song) {
  if (playtimes--) {
    if (song == 1) {
      file = new AudioFileSourceLittleFS("Hello.mp3");
    } else if (song == 2) {
      file = new AudioFileSourceLittleFS("Nicetomeetyou.mp3");
    } else if (song == 3) {
      file = new AudioFileSourceLittleFS("goodbye.mp3");
    } else if (song == 4) {
      file = new AudioFileSourceLittleFS("1.mp3");
    } else if (song == 5) {
      file = new AudioFileSourceLittleFS("onfoot.mp3");
    } else if (song == 6) {
      file = new AudioFileSourceLittleFS("hello_cn.mp3");
    } else if (song == 0) {
      file = new AudioFileSourceLittleFS("split.mp3");
    }
    out = new AudioOutputI2SNoDAC(6);
    out->SetGain(4.0);  //Volume Setup (0-4.0)
    mp3 = new AudioGeneratorMP3();
    mp3->begin(file, out);
    if (mp3->isRunning() && song > 0) {
      if (!mp3->loop() && song > 0) {
        delete file;
        delete mp3;
        out->flush();
        out->stop();
      }
    } else {
      delete file;
      delete mp3;
      out->flush();
      out->stop();
      pinMode(6, OUTPUT);
      digitalWrite(6, LOW);
    }
  }
}

//Ultrasonic robot
void Ultrasonic_Avoid() {
  if (Distance <= 15) {
    Bipedal_Robot.walk(2, 1000, -1);  // BACKWARD x2
    Bipedal_Robot.turn(3, 1000, -1);  // LEFT x3
  }
  Bipedal_Robot.walk(1, 1500, 1);  // FORWARD x1
}

void dance() {
  dance_steps = dance_steps + 1;
  staticEmtions(21);
  if (moveId > 0) {
    switch (dance_steps) {
      case 0:
        dance_steps = 0;
        break;
      case 1:
        Bipedal_Robot.jitter(1, 750, 20);
        break;
      case 2:
        Bipedal_Robot.jitter(1, 750, 20);
        break;
      case 3:
        Bipedal_Robot.crusaito(1, 800, 30, 1);
        break;
      case 4:
        Bipedal_Robot.crusaito(1, 800, 30, -1);
        break;
      case 5:
        Bipedal_Robot.crusaito(1, 800, 30, 1);
        Bipedal_Robot.home();
        delay(300);
        break;
      case 6:
        Bipedal_Robot.walk(1, 1500, -1);
        break;
      case 7:
        Bipedal_Robot.walk(1, 1000, 1);
        break;
      case 8:
        Bipedal_Robot.walk(1, 1000, 1);
        Bipedal_Robot.home();
        break;
      case 9:
        Bipedal_Robot.moonwalker(1, 600, 30, 1);
        break;
      case 10:
        Bipedal_Robot.moonwalker(1, 600, 30, -1);
        break;
      case 11:
        Bipedal_Robot.moonwalker(1, 600, 30, 1);
        break;
      case 12:
        Bipedal_Robot.moonwalker(1, 600, 30, -1);
        Bipedal_Robot.home();
        delay(100);
        break;
      case 13:
        Bipedal_Robot.walk(1, 1500, -1);
        Bipedal_Robot.home();
        delay(100);
        break;
    }
    if (dance_steps > 14) {
      dance_steps = 0;
    }
  } else {
    Resting = 1;
    moveId = 0;
    dance_steps = 0;
  }
}

void receiveMovement() {
  sendAck();
  ws2812_task_mode = 0;
  emotion_task_mode = 0;
  if (Bipedal_Robot.getRestState() == true) Bipedal_Robot.setRestState(false);
  isServoResting = Bipedal_Robot.getRestState();
  char *arg;
  arg = SCmd.next();
  if (arg != NULL) {
    moveId = atoi(arg);
    if (moveId == 0) {
      Resting = 1;
    }
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

void move(int movemode) {
  // bool manualMode = false;
  switch (movemode) {
    case 0:
      Bipedal_Robot.home();
      if (Resting == 1) {
        Bipedal_Robot.setRestState(false);
        Bipedal_Robot.home();
        Resting = 0;
      }
      break;
    case 1:
      staticEmtions(21);
      Bipedal_Robot.walk(1, T, 1);
      break;
    case 2:
      staticEmtions(21);
      Bipedal_Robot.walk(1, T, -1);
      break;
    case 3:
      staticEmtions(21);
      Bipedal_Robot.turn(1, T, 1);
      break;
    case 4:
      staticEmtions(21);
      Bipedal_Robot.turn(1, T, -1);
      break;
    case 5:
      Bipedal_Robot.updown(1, 2000, 30);
      break;
    case 6: Bipedal_Robot.moonwalker(1, T, moveSize, 1); break;
    case 7: Bipedal_Robot.moonwalker(1, T, moveSize, -1); break;
    case 8: Bipedal_Robot.swing(1, T, moveSize); break;
    case 9: Bipedal_Robot.crusaito(1, T, moveSize, 1); break;
    case 10: Bipedal_Robot.crusaito(1, T, moveSize, -1); break;
    case 11: Bipedal_Robot.jump(1, T); break;
    case 12: Bipedal_Robot.flapping(1, T, moveSize, 1); break;
    case 13: Bipedal_Robot.flapping(1, T, moveSize, -1); break;
    case 14: Bipedal_Robot.tiptoeSwing(1, T, moveSize); break;
    case 15: Bipedal_Robot.bend(1, T, 1); break;
    case 16: Bipedal_Robot.bend(1, T, -1); break;
    case 17: Bipedal_Robot.shakeLeg(1, T, 1); break;
    case 18:
      staticEmtions(21);
      Bipedal_Robot.swing(1, 1100, 50); 
      Bipedal_Robot.home();
      Resting = 1;
      moveId = 0;
      clearEmtions();
      break;
    case 19:
      staticEmtions(22);
      Bipedal_Robot.moonwalker(1, 1550, 50, 1);  //1541
      Bipedal_Robot.home();
      Resting = 1;
      moveId = 0;
      clearEmtions();
      break;
    case 20:
      staticEmtions(23);
      Bipedal_Robot.ascendingTurn(2, 700, 12);  //1332
      clearEmtions();
      delay(100);
      Bipedal_Robot.home();
      Resting = 1;
      moveId = 0;
      break;
    case 21:
      Ultrasonic_Avoid();
      break;
    case 22:
      dance();
      break;
    default:
      break;
  }
}

void sendAck() {
  delay(5);
  Serial.flush();
}
void sendFinalAck() {
  delay(5);
  Serial.flush();
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
void setup() {
  Serial.begin(115200);
  BTserial.begin(115200);
  delay(1500);
  EEPROM.begin(512);                                                             //Initialize the ultrasonic module
  Bipedal_Robot.init(LeftLegPin, RightLegPin, LeftFootPin, RightFootPin, true);  //Set the servo pins
  Bipedal_Robot.Buzzer_init(BuzzerPin);

  Bipedal_Robot.saveTrimsOnEEPROM();
  Bipedal_Robot.home();
  delay(50);
  Buzzer_Setup();  //Initialize the buzzer
  WS2812_Setup();  //WS2812 initialization
  Emotion_Setup();
  Ultrasonic_Setup();

  SCmd.addCommand("S", receiveStop);
  SCmd.addCommand("C", receiveLED);
  SCmd.addCommand("D", recieveBuzzer);
  SCmd.addCommand("A", receiveMovement);
  SCmd.addCommand("B", receiveEmotion);
  SCmd.addCommand("H", receiveAvoid);
  SCmd.addCommand("M", receiveMusic);
  SCmd.addDefaultHandler(receiveStop);
}

void loop() {
  SCmd.readSerial();
  Emotion_Detection();
  WS2812_Show(ws2812_task_mode);  //Car color lights display function
  if (millis() - lastUploadVoltageTime > UPLOAD_VOL_TIME) {
    upLoadVoltageToApp();
    lastUploadVoltageTime = millis();
  }
  if (millis() - lastUploadDistanceTime > 200) {
    if (ultrasonicavoidMode == 1) {
      upLoadDistanceToApp();
    }
    lastUploadDistanceTime = millis();
  }
  if (Check_Module_value == MATRIX_IS_EXIST) {
    Emotion_Show(emotion_task_mode);  //Led matrix display function
  }
  move(moveId);
}

void setup1() {
  delay(2000);
  playmusic(1, 2);
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
}
void loop1() {
  songmusic();
}
