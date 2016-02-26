#include <Arduino.h>




#define FORWARD         1
#define BACKWARD        2


void setup();

void moveMotor(int direction, int PWM_val, long pos);

void motorForward(int PWM_val);

void motorBackward(int PWM_val);

void motorSleep();

void readEncoder();

void loop();
#line 8 "thing.ino"
unsigned long resetInterval = 400;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

int encoderTicks = 448;
int ledPin =  13;

int pwmPin = 11;
int motorPin1 = 7;
int motorPin2 = 9;

int encodPin1 = 2;
int encodPin2 = 8;

int potPin = 0;
int val = 0;

volatile long encoderTick = 0;
volatile long setPoint = 0;
volatile boolean runState = true;

void setup() {
pinMode(motorPin1, OUTPUT);
pinMode(motorPin2, OUTPUT);
pinMode(pwmPin, OUTPUT);

pinMode(encodPin1, INPUT);
pinMode(encodPin2, INPUT);
attachInterrupt(0, readEncoder, CHANGE);

Serial.begin(9600);
Serial.println("Encoder Test:");

encoderTick = 0;
setPoint = 100;

}

void moveMotor(int direction, int PWM_val, long pos) {

if (runState == true) {
if(direction==FORWARD)          motorForward(PWM_val);
else if(direction==BACKWARD)    motorBackward(PWM_val);
}
else {
motorSleep();
}
}

void motorForward(int PWM_val)  {
digitalWrite(ledPin,HIGH);
analogWrite(pwmPin, PWM_val);
digitalWrite(motorPin1, LOW);
digitalWrite(motorPin2, HIGH);
runState = true;
}

void motorBackward(int PWM_val)  {
digitalWrite(ledPin,HIGH);
analogWrite(pwmPin, PWM_val);
digitalWrite(motorPin1, HIGH);
digitalWrite(motorPin2, LOW);
runState = true;
}

void motorSleep()  {
digitalWrite(ledPin,LOW);
analogWrite(pwmPin, 0);
digitalWrite(motorPin1, LOW);
digitalWrite(motorPin2, LOW);
runState = false;
}

void readEncoder(){
if (((PIND&B0000100)>>2) == HIGH) {
if ((PINB&B0000001) == LOW) {
encoderTick-- ;
}
else {
encoderTick++ ;
}
}
else {
if ((PINB&B0000001) == LOW) {
encoderTick++ ;
}
else {
encoderTick-- ;
}
}
if (encoderTick > setPoint) {
runState = false;

}
}

void loop() {

val = analogRead(potPin);
Serial.println(val);
Serial.println(" ");
val = map(val,0,1023,-255,255);
Serial.println(val);

delay(500);

analogWrite(pwmPin,val);
if (val > 0) {
moveMotor(FORWARD, val, 10);
}
else {
moveMotor(BACKWARD, val, 10);
}

delay(500);


}