#include <Arduino.h>
#include <SoftwareSerial.h>


void setup();

void loop();
#line 3 "thing.ino"
const int intPin  = 2;
const int TxPin   = 6;
const int led     = 13;
const int potPin  = 2;
int potVal  = 0;
int val  = 0;

SoftwareSerial mySerial = SoftwareSerial(255, TxPin);

void setup() {



pinMode(led, OUTPUT);
pinMode(TxPin, OUTPUT);
digitalWrite(TxPin, HIGH);

pinMode(intPin, INPUT);

mySerial.begin(9600);
delay(100);
mySerial.write(12);
mySerial.write(17);
delay(5);
mySerial.print("Read analog in");
mySerial.write(13);
mySerial.print("...!");
mySerial.write(212);
mySerial.write(220);
delay(3000);


}

void loop() {
mySerial.write(12);
mySerial.write(13);
mySerial.print("thing");


digitalWrite(led, HIGH);
delay(100);
digitalWrite(led, LOW);
delay(100);

}
