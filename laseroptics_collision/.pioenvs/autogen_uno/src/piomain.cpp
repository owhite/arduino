#include <Arduino.h>
#include <SPI.h>

#ifdef abs
#undef abs
#endif


#define abs(x) ((x)>0?(x):-(x))



void setup();

void loop();
void reportError();
int getMoveState();
void runMotors();
int readSPI();
#line 11 "thing.ino"
const int beepPin = 8;
const int stepPin    = 2;
const int dirPin     = 3;


const int chipSelectPin = 10;
const int upSwitchPin = 5;
const int dnSwitchPin = 4;


const int UP   = 1;
const int STOP = 2;
const int DOWN = 3;
int motorState = STOP;
const int delayTime = 1000;
unsigned long setMotorTimer = 0;
unsigned long incomingDuration = 0;
int incomingDirection = 0;


unsigned long resetInterval = 200;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
const int positionThreshold = 2;
int oldPosition = 0;
int curPosition = 0;
int errorState = 1;

void setup() {

Serial.begin(19200);
delay(2000);


pinMode(stepPin, OUTPUT);
digitalWrite(stepPin, LOW);
pinMode(dirPin, OUTPUT);
digitalWrite(dirPin, LOW);


SPI.begin();
SPI.setDataMode(SPI_MODE3);
SPI.setClockDivider(SPI_CLOCK_DIV16);
SPI.setBitOrder(MSBFIRST);
pinMode(chipSelectPin, OUTPUT);
delay(100);


pinMode(dnSwitchPin, INPUT_PULLUP);
pinMode(upSwitchPin, INPUT_PULLUP);


tone(beepPin, 440, 100);
}

void loop() {
motorState = getMoveState();
if (motorState != STOP) {
delay(2);
runMotors();
}
else {

reportError();
}
}



void reportError() {


currentMillis = millis();
if ((currentMillis - previousMillis) >= resetInterval) {
curPosition = readSPI();


if (abs(oldPosition - curPosition) > positionThreshold) {
errorState = 0;
tone(beepPin, 840, 40);
}
else {
errorState = 1;
}
Serial.println(errorState);
previousMillis = currentMillis;
oldPosition = curPosition;
}
}


int getMoveState() {
int state = STOP;
char ch;







while (Serial.available() > 0) {
ch = Serial.read();
incomingDirection = Serial.parseInt();
incomingDuration = Serial.parseInt();
if (Serial.read() == '\n') {
setMotorTimer = millis();
if (ch != 'D') {
incomingDirection = STOP;
incomingDuration = 0;
}
}
}

currentMillis = millis();
if ((currentMillis - setMotorTimer) >= incomingDuration ||
(incomingDirection != UP && incomingDirection != DOWN) ||
incomingDuration == 0 ||
incomingDuration > 20000) {
incomingDirection = STOP;
incomingDuration = 0;
}

if (incomingDirection == UP) {
digitalWrite(dirPin, HIGH);
state = UP;
}
else if (incomingDirection == DOWN) {
digitalWrite(dirPin, LOW);
state = DOWN;
}
else if (!(digitalRead(upSwitchPin))) {
digitalWrite(dirPin, HIGH);
state = UP;
}
else if (!(digitalRead(dnSwitchPin))) {
digitalWrite(dirPin, LOW);
state = DOWN;
}
else {
state = STOP;
}

return(state);
}


void runMotors() {
if (motorState != STOP) {
digitalWrite(stepPin, HIGH);
delayMicroseconds(delayTime);
digitalWrite(stepPin, LOW);
delayMicroseconds(delayTime);
motorState = getMoveState();
}
}





int readSPI() {
int result = 0;
byte inByte = 0;

digitalWrite(chipSelectPin, LOW);
result = SPI.transfer(0x00);
inByte = SPI.transfer(0x00);
result = result << 3;
digitalWrite(chipSelectPin, HIGH);
inByte &= 0b11100000;
inByte = inByte >> 5;
return (result | inByte);
}
