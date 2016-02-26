#include <SoftwareSerial.h>
#include <SPI.h>

#ifdef abs
#undef abs
#endif

#define abs(x) ((x)>0?(x):-(x)) // SPI is incompatible with abs()

// pin 2 of the rotary encoder (clk) connects to Uno board pin 13
// pin 4 of the rotary encoder (digital out) --> Uno board pin 12 (miso)????
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
  // SPI settings for Bourns EMS22A50-B28-LS6
  SPI.begin();
  SPI.setDataMode(SPI_MODE3);
  SPI.setClockDivider(SPI_CLOCK_DIV16); // its slow
  SPI.setBitOrder(MSBFIRST);
  pinMode(chipSelectPin, OUTPUT);
  pinMode(dnSwitchPin, INPUT);
  pinMode(upSwitchPin, INPUT);
  delay(100);

  mySerial.begin(9600);
  delay(100);
  mySerial.write(12);                 // Clear             
  mySerial.write(17);                 // Turn backlight on
  delay(5);                           // Required delay
  mySerial.print("SPI input");  // First line
  mySerial.write(13);                 // Form feed
  mySerial.print("...!");   // Second line
  mySerial.write(209);                // Quarter note
  mySerial.write(220);                // A tone
  delay(500);                        
}

void loop() {
  currentMillis = millis();
  // testing if some length of time has gone by
  if ((currentMillis - previousMillis) >= resetInterval) {
    reportStatus();
    curPosition = readSPI();

    // only complain if the motor is not running
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
    // user just stopped requesting motor movement
    mySerial.write(208);                // 64 note
    mySerial.write(230);                // A tone
    delay(100);
    motorState = false;
  }
  if (buttonState != MIDDLE &&
      motorState == false) {
    // user just requested motor movement
    mySerial.write(208);                // 64 note
    mySerial.write(223);
    delay(100);
    motorState = true;
  }
}

void reportStatus() {
  mySerial.write(12);                 // Clear             
  if (buttonState == UP) {
    mySerial.print("Motor up");
  }
  if (buttonState == DOWN) {
    mySerial.print("Motor down");
  }
  if (buttonState == MIDDLE) {
    mySerial.print("Detecting");
  }
  mySerial.write(13);                 // Form feed
  mySerial.print(curPosition);
}


void throwError() {
  mySerial.write(208);                // 64 note
  mySerial.write(220);                // A tone
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

// code to read the SPI of Bourns EMS22A50-B28-LS6
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
  return (result | inByte); // add in the bits and return
}


