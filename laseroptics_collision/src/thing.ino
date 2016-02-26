#include <SPI.h>

#ifdef abs
#undef abs
#endif

// SPI is incompatible with abs() so do this:
#define abs(x) ((x)>0?(x):-(x)) 

// PIN STUFF
const int beepPin = 8; // piezo speaker
const int stepPin    = 2; // red wire
const int dirPin     = 3; // orange wire
// pin 2 of the rotary encoder (clk, purple) connects to Uno board pin 13
// pin 4 of the rotary encoder (digital out, grey) --> Uno board pin 12 (miso)
const int chipSelectPin = 10; // attaches to pin 6 of encoder, white
const int upSwitchPin = 5;    // for switch
const int dnSwitchPin = 4;    // for switch

// MOTOR STUFF
const int UP   = 1;
const int STOP = 2;
const int DOWN = 3;
int motorState = STOP;
const int delayTime = 1000; // millisec length of PWM to motor
unsigned long setMotorTimer = 0;
unsigned long incomingDuration = 0;
int incomingDirection = 0;

// ENCODER STUFF
unsigned long resetInterval = 200; // milli() interval between encoder checks
unsigned long previousMillis = 0;  // used in counting time
unsigned long currentMillis = 0;   // also used in counting time
const int positionThreshold = 2;   // delta between encoder position to throw an error
int oldPosition = 0;               // for monitoring position
int curPosition = 0;               // for monitoring position
int errorState = 1;                // error flag for encoder

void setup() {

  Serial.begin(19200);
  delay(2000);

  // stepper setup
  pinMode(stepPin, OUTPUT);
  digitalWrite(stepPin, LOW); 
  pinMode(dirPin, OUTPUT);
  digitalWrite(dirPin, LOW); 

  // SPI setup for Bourns EMS22A50-B28-LS6
  SPI.begin();
  SPI.setDataMode(SPI_MODE3);
  SPI.setClockDivider(SPI_CLOCK_DIV16); // its slow
  SPI.setBitOrder(MSBFIRST);
  pinMode(chipSelectPin, OUTPUT);
  delay(100);

  // other pins to play with
  pinMode(dnSwitchPin, INPUT_PULLUP);
  pinMode(upSwitchPin, INPUT_PULLUP);
  
  // say hi to the user
  tone(beepPin, 440, 100);
}

void loop() {
  motorState = getMoveState();
  if (motorState != STOP) {
    delay(2);
    runMotors();
  }
  else {
    // only look for a problem when the motor is not running. 
    reportError();
  }
}

// checks position of encoder and continuously
// reports good or bad news to the serial. 
void reportError() {
  // testing if some length of time has gone by
  // and when it does, check if encoder has changed a lot
  currentMillis = millis();
  if ((currentMillis - previousMillis) >= resetInterval) {
    curPosition = readSPI();
    // only complain if the motor is not running
    // if (motorState == STOP && 
    if (abs(oldPosition - curPosition) > positionThreshold) {
      errorState = 0;
      tone(beepPin, 840, 40); // alert user
    }
    else {
      errorState = 1;
    }
    Serial.println(errorState); // send news to serial
    previousMillis = currentMillis;
    oldPosition = curPosition;
  }
}

// test if user hit the switches or if linuxcnc has sent a request over the serial
int getMoveState() {
  int state = STOP; // err on the side of caution
  char ch;

  // see if linuxcnc is talking to the arduino
  // this will over ride button status if something comes in. 
  // format recieved, e.g., "D 1 100\n"
  // the first letter must be "D" 
  // the first digit is 1 = UP, 2 = STOP, or 3 = DOWN
  // second digit is the time in milliseconds to perform the move
  while (Serial.available() > 0) {
    ch = Serial.read();
    incomingDirection = Serial.parseInt(); // collects space or comma separated digits
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
  if ((currentMillis - setMotorTimer) >= incomingDuration || // timer has run out
      (incomingDirection != UP && incomingDirection != DOWN) || // user made a mistake
      incomingDuration == 0 || // it was never set or is back to zero
      incomingDuration > 20000) { // user set something too high
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

// stay in this loop as long as needed
void runMotors() {
  if (motorState != STOP) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(delayTime);
    motorState = getMoveState();
  }
}

// code to read the SPI of Bourns encoder p/n EMS22A50-B28-LS6
// this is not consistent with the docs that seems to state
// that a whole byte of position data comes in, but it needed
// to get shifted over 3 bits. 
int readSPI() {
  int result = 0;
  byte inByte = 0;

  digitalWrite(chipSelectPin, LOW);  // assert chip select
  result = SPI.transfer(0x00);       // read a byte
  inByte = SPI.transfer(0x00);       // read a byte
  result = result << 3; // doesnt makes sense but make room for 3 more bits
  digitalWrite(chipSelectPin, HIGH); // deselect
  inByte &= 0b11100000; // mask off the first three bits
  inByte = inByte >> 5;  
  return (result | inByte); // add in the bits and return
}

