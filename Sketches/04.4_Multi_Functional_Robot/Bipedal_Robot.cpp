#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#include <pins_arduino.h>
#endif
#include "Bipedal_Robot.h"
#include "Oscillator.h"

void Bipedal_Robot::init(int YL, int YR, int RL, int RR, bool load_calibration) {

  servo_pins[0] = YL;
  servo_pins[1] = YR;
  servo_pins[2] = RL;
  servo_pins[3] = RR;
  attachServos();
  isBipedalRobotResting = false;
  if (load_calibration) {
    for (int i = 0; i < 4; i++) {
      int servo_trim = EEPROM.read(i);
      if (servo_trim > 128) servo_trim -= 256;
      servo[i].SetTrim(servo_trim);
    }
  }
}
void Bipedal_Robot::Buzzer_init(int Buzzer) {
  //Buzzer pin:
  pinBuzzer = Buzzer;
  pinMode(Buzzer, OUTPUT);
}
///////////////////////////////////////////////////////////////////
//-- ATTACH & DETACH FUNCTIONS ----------------------------------//
///////////////////////////////////////////////////////////////////
void Bipedal_Robot::attachServos() {
  servo[0].attach(servo_pins[0]);
  servo[1].attach(servo_pins[1]);
  servo[2].attach(servo_pins[2]);
  servo[3].attach(servo_pins[3]);
}

void Bipedal_Robot::detachServos() {
  servo[0].detach();
  servo[1].detach();
  servo[2].detach();
  servo[3].detach();
}

///////////////////////////////////////////////////////////////////
//-- OSCILLATORS TRIMS ------------------------------------------//
///////////////////////////////////////////////////////////////////
void Bipedal_Robot::setTrims(int YL, int YR, int RL, int RR) {
  servo[0].SetTrim(YL);
  servo[1].SetTrim(YR);
  servo[2].SetTrim(RL);
  servo[3].SetTrim(RR);
}

void Bipedal_Robot::saveTrimsOnEEPROM() {

  for (int i = 0; i < 4; i++) {
    EEPROM.write(i, servo[i].getTrim());
  }
  EEPROM.commit();
}

///////////////////////////////////////////////////////////////////
//-- BASIC MOTION FUNCTIONS -------------------------------------//
///////////////////////////////////////////////////////////////////
void Bipedal_Robot::_moveServos(int time, int servo_target[]) {

  attachServos();
  if (getRestState() == true) {
    setRestState(false);
  }

  final_time = millis() + time;
  if (time > 10) {
    for (int i = 0; i < 4; i++) increment[i] = (servo_target[i] - servo[i].getPosition()) / (time / 10.0);

    for (int iteration = 1; millis() < final_time; iteration++) {
      partial_time = millis() + 10;
      for (int i = 0; i < 4; i++) servo[i].SetPosition(servo[i].getPosition() + increment[i]);
      while (millis() < partial_time)
        ;  //pause
    }
  } else {
    for (int i = 0; i < 4; i++) servo[i].SetPosition(servo_target[i]);
    while (millis() < final_time);  //pause
  }

  // final adjustment to the target. if servo speed limiter is turned on, reaching to the goal may take longer than
  // requested time.
  bool f = true;
  while (f) {
    f = false;
    for (int i = 0; i < 4; i++) {
      if (servo_target[i] != servo[i].getPosition()) {
        f = true;
        break;
      }
    }
    if (f) {
      for (int i = 0; i < 4; i++) {
        servo[i].SetPosition(servo_target[i]);
      }
      partial_time = millis() + 10;
      while (millis() < partial_time);  //pause
    }
  }
}

void Bipedal_Robot::_moveSingle(int position, int servo_number) {
  if (position > 180) position = 90;
  if (position < 0) position = 90;
  attachServos();
  if (getRestState() == true) {
    setRestState(false);
  }
  int servoNumber = servo_number;
  if (servoNumber == 0) {
    servo[0].SetPosition(position);
  }
  if (servoNumber == 1) {
    servo[1].SetPosition(position);
  }
  if (servoNumber == 2) {
    servo[2].SetPosition(position);
  }
  if (servoNumber == 3) {
    servo[3].SetPosition(position);
  }
}

void Bipedal_Robot::oscillateServos(int A[4], int O[4], int T, double phase_diff[4], float cycle = 1) {

  for (int i = 0; i < 4; i++) {
    servo[i].SetO(O[i]);
    servo[i].SetA(A[i]);
    servo[i].SetT(T);
    servo[i].SetPh(phase_diff[i]);
  }
  double ref = millis();
  for (double x = ref; x <= T * cycle + ref; x = millis()) {
    for (int i = 0; i < 4; i++) {
      servo[i].refresh();
    }
  }
}

void Bipedal_Robot::_execute(int A[4], int O[4], int T, double phase_diff[4], float steps = 1.0) {

  attachServos();
  if (getRestState() == true) {
    setRestState(false);
  }
  int cycles = (int)steps;
  //-- Execute complete cycles
  if (cycles >= 1)
    for (int i = 0; i < cycles; i++)
      oscillateServos(A, O, T, phase_diff);

  //-- Execute the final not complete cycle
  oscillateServos(A, O, T, phase_diff, (float)steps - cycles);
}

//-- HOME = Bipedal_Robot at rest position -------------------------------//
void Bipedal_Robot::home() {

  if (isBipedalRobotResting == false) {  //Go to rest position only if necessary

    int homes[4] = { 90, 90, 90, 90 };  //All the servos at rest position
    _moveServos(700, homes);            //Move the servos in half a second

    detachServos();
    isBipedalRobotResting = true;
    Serial.print("home-home");
  }
}

bool Bipedal_Robot::getRestState() {
  return isBipedalRobotResting;
}

void Bipedal_Robot::setRestState(bool state) {

  isBipedalRobotResting = state;
}

//-- PREDETERMINED MOTION SEQUENCES -----------------------------//
//-- Bipedal_Robot movement: Jump
//--  Parameters:
//--    steps: Number of steps
//--    T: Period
//---------------------------------------------------------
void Bipedal_Robot::jump(float steps, int T) {

  int up[] = { 90, 90, 150, 30 };
  _moveServos(T, up);
  int down[] = { 90, 90, 90, 90 };
  _moveServos(T, down);
}

//-- Bipedal_Robot gait: Walking  (forward or backward)
//--  Parameters:
//--    * steps:  Number of steps
//--    * T : Period
//--    * Dir: Direction: FORWARD / BACKWARD
void Bipedal_Robot::walk(float steps, int T, int dir) {
  //-- Oscillator parameters for walking
  //-- Hip sevos are in phase
  //-- Feet servos are in phase
  //-- Hip and feet are 90 degrees out of phase
  //--      -90 : Walk forward
  //--       90 : Walk backward
  //-- Feet servos also have the same offset (for tiptoe a little bit)
  int A[4] = { 45, 45, 40, 40 };
  int O[4] = { 0, 0, 0, 0 };
  double phase_diff[4] = { 0, 0, DEG2RAD(dir * -90), DEG2RAD(dir * -90) };

  //-- Let's oscillate the servos!
  _execute(A, O, T, phase_diff, steps);
}

//-- Bipedal_Robot gait: Turning (left or right)
//--  Parameters:
//--   * Steps: Number of steps
//--   * T: Period
//--   * Dir: Direction: LEFT / RIGHT
void Bipedal_Robot::turn(float steps, int T, int dir) {

  //-- Same coordination than for walking (see Bipedal_Robot::walk)
  //-- The Amplitudes of the hip's oscillators are not igual
  //-- When the right hip servo amplitude is higher, the steps taken by
  //--   the right leg are bigger than the left. So, the robot describes an
  //--   left arc
  int A[4] = { 30, 30, 20, 20 };
  int O[4] = { 0, 0, 4, -4 };
  double phase_diff[4] = { 0, 0, DEG2RAD(-90), DEG2RAD(-90) };

  if (dir == LEFT) {
    A[0] = 45;  //-- Left hip servo
    A[1] = 15;  //-- Right hip servo
  } else {
    A[0] = 15;
    A[1] = 45;
  }

  //-- Let's oscillate the servos!
  _execute(A, O, T, phase_diff, steps);
}

//-- Bipedal_Robot gait: Lateral bend
//--  Parameters:
//--    steps: Number of bends
//--    T: Period of one bend
//--    dir: RIGHT=Right bend LEFT=Left bend
void Bipedal_Robot::bend(int steps, int T, int dir) {

  //Parameters of all the movements. Default: Left bend
  int bend1[4] = { 90, 90, 62, 35 };
  int bend2[4] = { 90, 90, 62, 105 };
  int homes[4] = { 90, 90, 90, 90 };

  //Time of one bend, constrained in order to avoid movements too fast.
  //T=max(T, 600);
  //Changes in the parameters if right direction is chosen
  if (dir == -1) {
    bend1[2] = 180 - 35;
    bend1[3] = 180 - 60;  //Not 65. Bipedal_Robot is unbalanced
    bend2[2] = 180 - 105;
    bend2[3] = 180 - 60;
  }

  //Time of the bend movement. Fixed parameter to avoid falls
  int T2 = 800;

  //Bend movement
  for (int i = 0; i < steps; i++) {
    _moveServos(T2 / 2, bend1);
    _moveServos(T2 / 2, bend2);
    delay(T * 0.8);
    _moveServos(500, homes);
  }
}

//-- Bipedal_Robot gait: Shake a leg
//--  Parameters:
//--    steps: Number of shakes
//--    T: Period of one shake
//--    dir: RIGHT=Right leg LEFT=Left leg
void Bipedal_Robot::shakeLeg(int steps, int T, int dir) {

  //This variable change the amount of shakes
  int numberLegMoves = 2;

  //Parameters of all the movements. Default: Right leg
  int shake_leg1[4] = { 90, 90, 58, 35 };
  int shake_leg2[4] = { 90, 90, 58, 120 };
  int shake_leg3[4] = { 90, 90, 58, 60 };
  int homes[4] = { 90, 90, 90, 90 };

  //Changes in the parameters if left leg is chosen
  if (dir == -1) {
    shake_leg1[2] = 180 - 35;
    shake_leg1[3] = 180 - 58;
    shake_leg2[2] = 180 - 120;
    shake_leg2[3] = 180 - 58;
    shake_leg3[2] = 180 - 60;
    shake_leg3[3] = 180 - 58;
  }

  //Time of the bend movement. Fixed parameter to avoid falls
  int T2 = 1000;
  //Time of one shake, constrained in order to avoid movements too fast.
  T = T - T2;
  T = max(T, 200 * numberLegMoves);

  for (int j = 0; j < steps; j++) {
    //Bend movement
    _moveServos(T2 / 2, shake_leg1);
    _moveServos(T2 / 2, shake_leg2);

    //Shake movement
    for (int i = 0; i < numberLegMoves; i++) {
      _moveServos(T / (2 * numberLegMoves), shake_leg3);
      _moveServos(T / (2 * numberLegMoves), shake_leg2);
    }
    _moveServos(500, homes);  //Return to home position
  }

  delay(T);
}

//-- Bipedal_Robot movement: up & down
//--  Parameters:
//--    * steps: Number of jumps
//--    * T: Period
//--    * h: Jump height: SMALL / MEDIUM / BIG
//--              (or a number in degrees 0 - 90)
void Bipedal_Robot::updown(float steps, int T, int h) {

  //-- Both feet are 180 degrees out of phase
  //-- Feet amplitude and offset are the same
  //-- Initial phase for the right foot is -90, so that it starts
  //--   in one extreme position (not in the middle)
  int A[4] = { 0, 0, h, h };
  int O[4] = { 0, 0, h, -h };
  double phase_diff[4] = { 0, 0, DEG2RAD(-90), DEG2RAD(90) };

  //-- Let's oscillate the servos!
  _execute(A, O, T, phase_diff, steps);
}

//-- Bipedal_Robot movement: swinging side to side
//--  Parameters:
//--     steps: Number of steps
//--     T : Period
//--     h : Amount of swing (from 0 to 50 aprox)

void Bipedal_Robot::swing(float steps, int T, int h) {

  //-- Both feets are in phase. The offset is half the amplitude
  //-- It causes the robot to swing from side to side
  int A[4] = { 5, 5, h, h };// int A[4] = { 0, 0, h, h };
  int O[4] = { 0, 0, h / 2, -h / 2 };
  double phase_diff[4] = { 0, 0, DEG2RAD(0), DEG2RAD(0) };

  //-- Let's oscillate the servos!
  _execute(A, O, T, phase_diff, steps);
}

//-- Bipedal_Robot movement: swinging side to side without touching the floor with the heel
//--  Parameters:
//--     steps: Number of steps
//--     T : Period
//--     h : Amount of swing (from 0 to 50 aprox)
void Bipedal_Robot::tiptoeSwing(float steps, int T, int h) {

  //-- Both feets are in phase. The offset is not half the amplitude in order to tiptoe
  //-- It causes the robot to swing from side to side
  int A[4] = { 0, 0, h, h };
  int O[4] = { 0, 0, h, -h };
  double phase_diff[4] = { 0, 0, 0, 0 };

  //-- Let's oscillate the servos!
  _execute(A, O, T, phase_diff, steps);
}

//-- Bipedal_Robot gait: Jitter
//--  Parameters:
//--    steps: Number of jitters
//--    T: Period of one jitter
//--    h: height (Values between 5 - 25)
void Bipedal_Robot::jitter(float steps, int T, int h) {
  //-- Both feet are 180 degrees out of phase
  //-- Feet amplitude and offset are the same
  //-- Initial phase for the right foot is -90, so that it starts
  //--   in one extreme position (not in the middle)
  //-- h is constrained to avoid hit the feets
  h = min(25, h);
  int A[4] = { h, h, 0, 0 };
  int O[4] = { 0, 0, 0, 0 };
  double phase_diff[4] = { DEG2RAD(-90), DEG2RAD(90), 0, 0 };

  //-- Let's oscillate the servos!
  _execute(A, O, T, phase_diff, steps);
}

//-- Bipedal_Robot gait: Ascending & turn (Jitter while up&down)
//--  Parameters:
//--    steps: Number of bends
//--    T: Period of one bend
//--    h: height (Values between 5 - 15)
void Bipedal_Robot::ascendingTurn(float steps, int T, int h) {

  //-- Both feet and legs are 180 degrees out of phase
  //-- Initial phase for the right foot is -90, so that it starts
  //--   in one extreme position (not in the middle)
  //-- h is constrained to avoid hit the feets
  h = min(13, h);
  int A[4] = { h, h, h, h };
  int O[4] = { 0, 0, h + 4, -h + 4 };
  double phase_diff[4] = { DEG2RAD(-90), DEG2RAD(90), DEG2RAD(-90), DEG2RAD(90) };

  //-- Let's oscillate the servos!
  _execute(A, O, T, phase_diff, steps);
}

//-- Bipedal_Robot gait: Moonwalker. Bipedal_Robot moves like Michael Jackson
//--  Parameters:
//--    Steps: Number of steps
//--    T: Period
//--    h: Height. Typical valures between 15 and 40
//--    dir: Direction: LEFT / RIGHT
void Bipedal_Robot::moonwalker(float steps, int T, int h, int dir) {

  //-- This motion is similar to that of the caterpillar robots: A travelling
  //-- wave moving from one side to another
  //-- The two Bipedal_Robot's feet are equivalent to a minimal configuration. It is known
  //-- that 2 servos can move like a worm if they are 120 degrees out of phase
  //-- In the example of Bipedal_Robot, the two feet are mirrored so that we have:
  //--    180 - 120 = 60 degrees. The actual phase difference given to the oscillators
  //--  is 60 degrees.
  //--  Both amplitudes are equal. The offset is half the amplitud plus a little bit of
  //-   offset so that the robot tiptoe lightly

  int A[4] = { 5, 5, h, h };//int A[4] = { 0, 0, h, h };
  int O[4] = { 0, 0, h / 2 + 2, -h / 2 - 2 };
  int phi = -dir * 90;
  double phase_diff[4] = { 0, 0, DEG2RAD(phi), DEG2RAD(-60 * dir + phi) };

  //-- Let's oscillate the servos!
  _execute(A, O, T, phase_diff, steps);
}

//-- Bipedal_Robot gait: Crusaito. A mixture between moonwalker and walk
//--   Parameters:
//--     steps: Number of steps
//--     T: Period
//--     h: height (Values between 20 - 50)
//--     dir:  Direction: LEFT / RIGHT
void Bipedal_Robot::crusaito(float steps, int T, int h, int dir) {

  int A[4] = { 25, 25, h, h };
  int O[4] = { 0, 0, h / 2 + 4, -h / 2 - 4 };
  double phase_diff[4] = { 90, 90, DEG2RAD(0), DEG2RAD(-60 * dir) };

  //-- Let's oscillate the servos!
  _execute(A, O, T, phase_diff, steps);
}

//-- Bipedal_Robot gait: Flapping
//--  Parameters:
//--    steps: Number of steps
//--    T: Period
//--    h: height (Values between 10 - 30)
//--    dir: direction: FOREWARD, BACKWARD
void Bipedal_Robot::flapping(float steps, int T, int h, int dir) {

  int A[4] = { 12, 12, h, h };
  int O[4] = { 0, 0, h - 10, -h + 10 };
  double phase_diff[4] = { DEG2RAD(0), DEG2RAD(180), DEG2RAD(-90 * dir), DEG2RAD(90 * dir) };

  //-- Let's oscillate the servos!
  _execute(A, O, T, phase_diff, steps);
}

void Bipedal_Robot::_tone(float noteFrequency, long noteDuration, int silentDuration) {
  if (silentDuration == 0) { silentDuration = 1; }
  tone(Bipedal_Robot::pinBuzzer, noteFrequency, noteDuration);
  delay(noteDuration);  //milliseconds to microseconds
  //noTone(PIN_Buzzer);
  delay(silentDuration);
}

void Bipedal_Robot::bendTones(float initFrequency, float finalFrequency, float prop, long noteDuration, int silentDuration) {

  //Examples:
  //  bendTones (880, 2093, 1.02, 18, 1);
  //  bendTones (note_A5, note_C7, 1.02, 18, 0);

  if (silentDuration == 0) { silentDuration = 1; }

  if (initFrequency < finalFrequency) {
    for (int i = initFrequency; i < finalFrequency; i = i * prop) {
      _tone(i, noteDuration, silentDuration);
    }

  } else {

    for (int i = initFrequency; i > finalFrequency; i = i / prop) {
      _tone(i, noteDuration, silentDuration);
    }
  }
}

void Bipedal_Robot::sing(int songName) {
  switch (songName) {
    case S_disconnection:
      _tone(note_E5, 50, 30);
      _tone(note_A6, 55, 25);
      _tone(note_E6, 50, 10);
      break;
    case S_connection:
      _tone(note_E5, 50, 30);
      _tone(note_E6, 55, 25);
      _tone(note_A6, 60, 10);
      break;
    case S_buttonPushed:
      bendTones(note_E6, note_G6, 1.03, 20, 2);
      delay(30);
      bendTones(note_E6, note_D7, 1.04, 10, 2);
      break;
    case S_mode1:
      bendTones(note_E6, note_A6, 1.02, 30, 10);  //1318.51 to 1760
      break;
    case S_mode2:
      bendTones(note_G6, note_D7, 1.03, 30, 10);  //1567.98 to 2349.32
      break;
    case S_mode3:
      _tone(note_E6, 50, 100);  //D6
      _tone(note_G6, 50, 80);   //E6
      _tone(note_D7, 300, 0);   //G6
      break;
    case S_OhOoh:
      bendTones(880, 2000, 1.04, 8, 3);  //A5 = 880
      delay(200);
      break;
    case S_surprise:
      bendTones(800, 2150, 1.02, 10, 1);
      bendTones(2149, 800, 1.03, 7, 1);
      for (int i = 880; i < 2000; i = i * 1.04) {
        _tone(note_B5, 5, 10);
      }
      break;
    case S_OhOoh2:
      bendTones(1880, 3000, 1.03, 8, 3);
      delay(200);
      for (int i = 1880; i < 3000; i = i * 1.03) {
        _tone(note_C6, 10, 10);
      }
      break;
    case S_cuddly:
      bendTones(700, 900, 1.03, 16, 4);
      bendTones(899, 650, 1.01, 18, 7);
      break;
    case S_sleeping:
      bendTones(100, 500, 1.04, 10, 10);
      delay(500);
      bendTones(400, 100, 1.04, 10, 1);
      break;
    case S_happy:
      bendTones(1500, 2500, 1.05, 20, 8);
      bendTones(2499, 1500, 1.05, 25, 8);
      break;
    case S_superHappy:
      bendTones(2000, 6000, 1.05, 8, 3);
      delay(50);
      bendTones(5999, 2000, 1.05, 13, 2);
      break;
    case S_happy_short:
      bendTones(1500, 2000, 1.05, 15, 8);
      delay(100);
      bendTones(1900, 2500, 1.05, 10, 8);
      break;
    case S_sad:
      bendTones(880, 669, 1.02, 20, 200);
      break;
    case S_confused:
      bendTones(1000, 1700, 1.03, 8, 2);
      bendTones(1699, 500, 1.04, 8, 3);
      bendTones(1000, 1700, 1.05, 9, 10);
      break;
    case S_fart1:
      bendTones(1600, 3000, 1.02, 2, 15);
      break;
    case S_fart2:
      bendTones(2000, 6000, 1.02, 2, 20);
      break;
    case S_fart3:
      bendTones(1600, 4000, 1.02, 2, 20);
      bendTones(4000, 3000, 1.02, 2, 20);
      break;
  }
}

void Bipedal_Robot::enableServoLimit(int diff_limit) {
  for (int i = 0; i < 4; i++) {
    servo[i].SetLimiter(diff_limit);
  }
}

void Bipedal_Robot::disableServoLimit() {
  for (int i = 0; i < 4; i++) {
    servo[i].DisableLimiter();
  }
}
