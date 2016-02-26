#include "Encoder.h"
#include "PID_v1.h"
#include "DCMotorServo.h"

#define pinDir1 7
#define pinDir2 9
#define pinPwmOut 11
#define pinEncode1 3
#define pinEncode2 2
#define potPin 0

int val = 0;

#define encoderTicks 4000

DCMotorServo servo = DCMotorServo(pinDir1, pinDir2, pinPwmOut, pinEncode1, pinEncode2);

void setup() {
  servo.myPID->SetTunings(0.1,0.05,0.00);
  servo.setPWMSkip(50);
  servo.setAccuracy(100);
}


void loop() {
  static bool motor_go = 0;

  val = analogRead(potPin);
  val = map(val,0,1023,0 , encoderTicks);

  servo.run();
  servo.moveTo(val);
  motor_go = 1;

}
