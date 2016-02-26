#include <Arduino.h>






void setup();

void motorForward();

void motorReverse();

void motorSleep();

void loop();
#line 6 "thing.ino"
unsigned long resetInterval = 400;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

int ledPin =  13;
int pwmPin = 11;
int motorPin1 = 7;
int motorPin2 = 9;
int potPin = 0;

int val;
int oldVal;

void setup() {
pinMode(ledPin, OUTPUT);
pinMode(pwmPin, OUTPUT);
pinMode(motorPin1, OUTPUT);
pinMode(motorPin2, OUTPUT);
Serial.begin(9600);
}

void motorForward() {
digitalWrite(ledPin,HIGH);
digitalWrite(motorPin1,HIGH);
digitalWrite(motorPin2,LOW);
}

void motorReverse() {
digitalWrite(ledPin,HIGH);
digitalWrite(motorPin1,LOW);
digitalWrite(motorPin2,HIGH);
}

void motorSleep() {
digitalWrite(ledPin,LOW);
digitalWrite(motorPin1,LOW);
digitalWrite(motorPin2,LOW);
}

void loop() {
val = analogRead(potPin);
val = map(val,0,1023,-255,255);



currentMillis = millis();
if ((currentMillis - previousMillis) >= resetInterval) {
if (oldVal == val) {
motorSleep();
}
else {
if (val >= 0) {
analogWrite(pwmPin,val);
motorForward();
}
else {
val *= -1;
analogWrite(pwmPin,val);
motorReverse();
}
}
previousMillis = currentMillis;
}
oldVal = val;

delay(100);
}
