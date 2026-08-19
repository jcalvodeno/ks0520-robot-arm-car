/*
 KS0520 servo diagnostic tool (not a vendor lesson - a debugging utility for this repo).

 Test each of the 4 arm servos individually, in small steps, with an
 always-available emergency stop.

 IMPORTANT SAFETY NOTE:
 setup() pre-loads every servo's target to its angle[] value (see below)
 BEFORE any attach() call ever happens - this is what stops the Arduino
 Servo library's own built-in ~90 deg default (set in every Servo object's
 constructor) from ever being used. So the instant you select a servo
 (1-4), attach() drives it straight to whatever angle[] currently holds
 for that channel - NOT a fixed 90, but not zero movement either. If
 angle[] does not match where the servo actually physically is, that
 first move can still be large and fast in whatever direction is needed
 to reach it. There is no position feedback, so this sketch can never
 truly know where a servo physically is - only test a servo with its
 horn/linkage physically REMOVED from the shaft until you've confirmed,
 unloaded, that its full range is clear.

 Serial @ 9600 baud. In the Serial Monitor, set line ending to "No line
 ending" so each keypress sends immediately.

 Commands:
   1 / 2 / 3 / 4   select servo (1=base A1, 2=left/shoulder A0, 3=right/elbow D8, 4=claw D9)
   +               nudge selected servo +2 degrees
   -               nudge selected servo -2 degrees
   c               print selected servo's current tracked angle
   d               detach selected servo (goes limp - safe to hand-rotate)
   x               EMERGENCY STOP: detach all four servos immediately
*/
#include <Servo.h>

const int NUM_SERVOS = 4;
const int PINS[NUM_SERVOS] = { A1, A0, 8, 9 };
const char* NAMES[NUM_SERVOS] = {
  "1 base (A1)",
  "2 left/shoulder (A0)",
  "3 right/elbow (D8)",
  "4 claw (D9)"
};
const int STEP = 2;

Servo servos[NUM_SERVOS];
bool attachedFlag[NUM_SERVOS] = { false, false, false, false };
int angle[NUM_SERVOS] = { 90, 90, 90, 90 };
int selected = -1;

void printMenu() {
  Serial.println();
  Serial.println(F("--- Servo diagnostic ---"));
  Serial.println(F("1/2/3/4 = select servo | + / - = nudge 2 deg | c = show angle | d = detach selected | x = EMERGENCY STOP (detach all)"));
  Serial.println(F("Test each servo with its horn OFF first. Selecting it moves it to its angle[] starting value immediately (edit angle[] in the code to change it)."));
}

void detachAll() {
  for (int i = 0; i < NUM_SERVOS; i++) {
    if (attachedFlag[i]) {
      servos[i].detach();
      attachedFlag[i] = false;
    }
  }
  Serial.println(F("[ALL SERVOS DETACHED]"));
}

void selectServo(int idx) {
  selected = idx;
  if (!attachedFlag[selected]) {
    servos[selected].attach(PINS[selected]);
    attachedFlag[selected] = true;
    Serial.print(F("Attached "));
    Serial.print(NAMES[selected]);
    Serial.print(F(" -- moving to its angle[] starting value NOW ("));
    Serial.print(angle[selected]);
    Serial.println(F(" deg). Watch it."));
  } else {
    Serial.print(F("Selected "));
    Serial.println(NAMES[selected]);
  }
  Serial.print(F("Tracked angle: "));
  Serial.println(angle[selected]);
}

void nudge(int delta) {
  if (selected < 0) {
    Serial.println(F("No servo selected. Press 1-4 first."));
    return;
  }
  angle[selected] = constrain(angle[selected] + delta, 0, 180);
  servos[selected].write(angle[selected]);
  Serial.print(NAMES[selected]);
  Serial.print(F(" -> "));
  Serial.println(angle[selected]);
}

void setup() {
  Serial.begin(9600);
  // Pre-load each servo's target BEFORE attach() ever runs. Servo::write()
  // only stores the value (servos[channel].ticks) - it does not produce any
  // output on its own, since output only happens for channels attach() has
  // marked active. So calling write() here, while every channel is still
  // inactive, means attach() (called later, from selectServo()) starts
  // outputting THIS value from its very first pulse - not the Servo
  // library's built-in ~90 deg default, which only applies to a channel
  // that reaches attach() with its target never having been written.
  for (int i = 0; i < NUM_SERVOS; i++) {
    servos[i].write(angle[i]);
  }
  printMenu();
}

void loop() {
  if (Serial.available() > 0) {
    char c = Serial.read();
    switch (c) {
      case '1': case '2': case '3': case '4':
        selectServo(c - '1');
        break;
      case '+':
        nudge(STEP);
        break;
      case '-':
        nudge(-STEP);
        break;
      case 'c':
        if (selected >= 0) {
          Serial.print(NAMES[selected]);
          Serial.print(F(" tracked angle: "));
          Serial.println(angle[selected]);
        } else {
          Serial.println(F("No servo selected."));
        }
        break;
      case 'd':
        if (selected >= 0 && attachedFlag[selected]) {
          servos[selected].detach();
          attachedFlag[selected] = false;
          Serial.print(NAMES[selected]);
          Serial.println(F(" detached (now limp - safe to hand-rotate)."));
        }
        break;
      case 'x':
        detachAll();
        break;
      default:
        break;
    }
  }
}
