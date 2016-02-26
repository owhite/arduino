#include <Arduino.h>
#include <SoftwareSerial.h>
#include <SPI.h>




void setup();

int readSPI();

void loop();
#line 6 "thing.ino"
const int txPin    = 6;
const int chipSelectPin = 10;

SoftwareSerial mySerial = SoftwareSerial(255, txPin);

void setup() {

SPI.begin();
SPI.setDataMode(SPI_MODE3);
SPI.setClockDivider(SPI_CLOCK_DIV32);
SPI.setBitOrder(MSBFIRST);


pinMode(chipSelectPin, OUTPUT);
delay(100);

mySerial.begin(9600);
delay(100);
mySerial.write(12);
mySerial.write(17);
delay(5);
mySerial.print("Reading SPI2:");
mySerial.write(13);
mySerial.print("...!");
mySerial.write(209);
mySerial.write(220);
delay(1000);
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
return(result | inByte);

}

void loop() {

mySerial.write(12);
mySerial.print("Reading SPI2:");
mySerial.write(13);
mySerial.print(readSPI());
delay(400);
}

