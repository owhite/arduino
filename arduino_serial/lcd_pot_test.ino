#include <SoftwareSerial.h>

const int TxPin   = 6;
const int led     = 13;
const int potPin  = 2;
int potVal  = 0;

SoftwareSerial mySerial = SoftwareSerial(255, TxPin);

void setup() {
    

  // the setup routine runs once when you press reset:
  pinMode(led, OUTPUT);     

  pinMode(TxPin, OUTPUT);
  digitalWrite(TxPin, HIGH);
  
  mySerial.begin(19200);
  delay(100);
  mySerial.write(12);                 // Clear             
  mySerial.write(17);                 // Turn backlight on
  delay(5);                           // Required delay
  mySerial.print("Read analog in");  // First line
  mySerial.write(13);                 // Form feed
  mySerial.print("...!");   // Second line
  mySerial.write(212);                // Quarter note
  mySerial.write(220);                // A tone
  delay(3000);                        // Wait 3 seconds
  // mySerial.write(18);                 // Turn backlight off

}

void loop() {
  mySerial.write(12);                 // Clear             
  potVal = analogRead(potPin);
  mySerial.print(potVal);

  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(100);               // wait for a second

}
