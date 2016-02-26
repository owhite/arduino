// this is a test of a DC motor in combination w/ MC338870 motor board
// it tests if the pot has changed, if it has it sets motor speed and moves.
// does not use the encoder so there's no control of motor turning distance

#define FORWARD         1                       // direction of rotation
#define BACKWARD        2                       // direction of rotation

unsigned long resetInterval = 400;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

int encoderTicks = 448;
int ledPin =  13;    // LED 

int pwmPin = 11;     // PWM Motor driver (/D2 pin on MC338870 motor board)
int motorPin1 = 7;   // Motor pin 1 (IN1 on motor board)
int motorPin2 = 9;   // Motor pin 2 (IN2 on motor board)

int encodPin1 = 2;
int encodPin2 = 8;

int potPin = 0;      // Potentiometer pin on analog input 0
int val = 0;

volatile long encoderTick = 0;
volatile long setPoint = 0;
volatile boolean runState = true;   // motor moves

void setup() {
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(pwmPin, OUTPUT);

  pinMode(encodPin1, INPUT);  
  pinMode(encodPin2, INPUT);  
  attachInterrupt(0, readEncoder, CHANGE);  // encoder pin on interrupt 0 - pin 2

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
  if (((PIND&B0000100)>>2) == HIGH) {   // found a low-to-high on channel A
    if ((PINB&B0000001) == LOW) {       // check channel B to see which way
      encoderTick-- ;         // CCW
    } 
    else {
      encoderTick++ ;         // CW
    }
  }
  else {                            // found a high-to-low
    if ((PINB&B0000001) == LOW) {   // check channel B to see which way
      encoderTick++ ;          // CW
    } 
    else {
      encoderTick-- ;          // CCW
    }
  } 
  if (encoderTick > setPoint) {
    runState = false;
    // motorSleep();
  }
}

void loop() {

  val = analogRead(potPin);
  Serial.println(val);
  Serial.println(" ");
  val = map(val,0,1023,-255,255);
  Serial.println(val);

  delay(500);

  analogWrite(pwmPin,val); // see docs, this sets PWM out
  if (val > 0) {
    moveMotor(FORWARD, val, 10);
  }
  else {
    moveMotor(BACKWARD, val, 10);
  }

  delay(500);

  // moveMotor(FORWARD, 220, 100);
}
