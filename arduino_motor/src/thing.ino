// this is a test of a DC motor in combination w/ MC338870 motor board
// it tests if the pot has changed, if it has it sets motor speed and moves.
// does not use the encoder so there's no control of motor turning distance


unsigned long resetInterval = 400;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

int ledPin =  13;    // LED 
int pwmPin = 11;     // PWM Motor driver (/D2 pin on MC338870 motor board)
int motorPin1 = 7;   // Motor pin 1 (IN1 on motor board)
int motorPin2 = 9;   // Motor pin 2 (IN2 on motor board)
int potPin = 0;      // Potentiometer pin on analog input 0

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

  // if some length of time has gone by
  // check to position has changed at all
  currentMillis = millis();
  if ((currentMillis - previousMillis) >= resetInterval) {
    if (oldVal == val) {
      motorSleep();
    }
    else {
      if (val >= 0) {
	analogWrite(pwmPin,val); // see docs, this sets PWM out
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

