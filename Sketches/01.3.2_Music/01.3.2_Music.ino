/**********************************************************************
  Filename    : Play music
  Description : Play music.
  Auther      : www.freenove.com
  Modification: 2023/11/07
**********************************************************************/
#include <Arduino.h>
#include "AudioFileSourceLittleFS.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"
#include "AudioFileSourceID3.h"

AudioGeneratorMP3 *mp3;
AudioFileSourceLittleFS *file;
AudioOutputI2SNoDAC *out;

void setup() {
  Serial.begin(115200);
  delay(1000);
  file = new AudioFileSourceLittleFS("1.mp3");
  out = new AudioOutputI2SNoDAC(6);
  out->SetGain(3);  //Volume Setup
  mp3 = new AudioGeneratorMP3();
  mp3->begin(file, out);
}
int a = 0;

void loop() {
  Serial.printf("loop1..\n");
  if (mp3->isRunning()) {
    if (!mp3->loop()) {
      mp3->stop();
      delete file;
      delete mp3;
      mp3 = new AudioGeneratorMP3();

    }
  } else {
    Serial.printf("MP3 done\n");
    pinMode(6, OUTPUT);
    digitalWrite(6, LOW);
  }
}
