#include <SoftwareSerial.h>
#include <SPI.h>

// pin 2 of the rotary encoder (clk) connects to Uno board pin 13
// pin 4 of the rotary encoder (digital out) --> Uno board pin 12 (miso)????
const int txPin    = 6;
const int chipSelectPin = 10;

SoftwareSerial mySerial = SoftwareSerial(255, txPin);

void setup() {
  // start the SPI library:
  SPI.begin();
  SPI.setDataMode(SPI_MODE3);
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  SPI.setBitOrder(MSBFIRST);

  // initalize the  data ready and chip select pins:
  pinMode(chipSelectPin, OUTPUT);
  delay(100);

  mySerial.begin(9600);
  delay(100);
  mySerial.write(12);                 // Clear             
  mySerial.write(17);                 // Turn backlight on
  delay(5);                           // Required delay
  mySerial.print("Reading SPI2:");  // First line
  mySerial.write(13);                 // Form feed
  mySerial.print("...!");   // Second line
  mySerial.write(209);                // Quarter note
  mySerial.write(220);                // A tone
  delay(1000);
}

int readSPI() {
  int result = 0;
  byte inByte = 0;

  digitalWrite(chipSelectPin, LOW);  // assert chip select
  result = SPI.transfer(0x00);       // read a byte
  inByte = SPI.transfer(0x00);       // read a byte
  digitalWrite(chipSelectPin, HIGH); // deselect
  result = result << 3; // shove em over to make room for 3 more
  inByte &= 0b11100000; // mask off the first three bits
  inByte = inByte >> 5; 
  return(result | inByte); 
  
}

void loop() {

  mySerial.write(12);                 // Clear             
  mySerial.print("Reading SPI2:");  // First line
  mySerial.write(13);                 // Form feed
  mySerial.print(readSPI()); 
  delay(400);
}

 
