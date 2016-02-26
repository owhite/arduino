#include <Arduino.h>
#include <SoftwareSerial.h>
#include <SPI.h>

#ifdef abs
#undef abs
#endif

#define abs(x) ((x)>0?(x):-(x))




void setup();

void loop();

void reportStatus();


void throwError();

int getButtonState();
int readSPI();
#line 12 "thing.ino"
const int chipSelectPin = 10;

const int txPin    = 6;
const int upSwitchPin = 5;
const int dnSwitchPin = 4;
const int potPin  = 2;

const int UP     = 1;
const int MIDDLE = 2;
const int DOWN   = 3;

int buttonState = MIDDLE;
int motorState = false;

unsigned long resetInterval = 400;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

int oldPosition = 0;
int curPosition = 0;

SoftwareSerial mySerial = SoftwareSerial(255, txPin);

int positionThreshold = 2;

void setup() {

SPI.begin();
SPI.setDataMode(SPI_MODE3);
SPI.setClockDivider(SPI_CLOCK_DIV16);
SPI.setBitOrder(MSBFIRST);
pinMode(chipSelectPin, OUTPUT);
pinMode(dnSwitchPin, INPUT);
pinMode(upSwitchPin, INPUT);
delay(100);

mySerial.begin(9600);
delay(100);
mySerial.write(12);
mySerial.write(17);
delay(5);
mySerial.print("SPI input");
mySerial.write(13);
mySerial.print("...!");
mySerial.write(209);
mySerial.write(220);
delay(500);
}

void loop() {
currentMillis = millis();

if ((currentMillis - previousMillis) >= resetInterval) {
reportStatus();
curPosition = readSPI();


if (motorState == false &&
abs(oldPosition - curPosition) > positionThreshold) {
throwError();
}
previousMillis = currentMillis;
oldPosition = curPosition;
}
buttonState = getButtonState();

if (buttonState == MIDDLE &&
motorState == true) {

mySerial.write(208);
mySerial.write(230);
delay(100);
motorState = false;
}
if (buttonState != MIDDLE &&
motorState == false) {

mySerial.write(208);
mySerial.write(223);
delay(100);
motorState = true;
}
}

void reportStatus() {
mySerial.write(12);
if (buttonState == UP) {
mySerial.print("Motor up");
}
if (buttonState == DOWN) {
mySerial.print("Motor down");
}
if (buttonState == MIDDLE) {
mySerial.print("Detecting");
}
mySerial.write(13);
mySerial.print(curPosition);
}


void throwError() {
mySerial.write(208);
mySerial.write(220);
}

int getButtonState() {
int val = analogRead(potPin);

return MIDDLE;

if (val < 600) {
return UP;
}
else if (val > 850) {
return DOWN;
}
else{
return MIDDLE;
}
}


int readSPI() {
int result = 0;
byte inByte = 0;

digitalWrite(chipSelectPin, LOW);
result = SPI.transfer(0x00);
inByte = SPI.transfer(0x00);
digitalWrite(chipSelectPin, HIGH);
result = result << 3;
inByte &= 0b11100000;
inByte = inByte >> 5;
return (result | inByte);
}

