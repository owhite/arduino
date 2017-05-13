// This is on the device that has the joysticks.
//   it has a simple job - transmit the data
//   from the joysticks. 

#define SERIAL_SPEED   115200
#define S_IDLE          1
#define S_POLLED        2
#define S_REPORT_STATUS 3

#define HARDWARE Serial1

#define MEM_LEN 12
uint8_t dataBuf[MEM_LEN]; 

uint8_t LEDPin = 13;

uint8_t btnPin1 = 15;
uint8_t btnPin2 = 14;

uint8_t joy1Pin  = 17;
uint8_t joy2Pin  = 18;
uint8_t joy3Pin  = 19;
uint8_t joy4Pin  = 20;

uint8_t btn_state1 = 0;
uint8_t btn_state2 = 0;

uint16_t joy1PinVal; uint16_t joy2PinVal;
uint16_t joy3PinVal; uint16_t joy4PinVal;

boolean blinkOn = false;
uint32_t blinkDelta = 0;
uint32_t blinkInterval = 200; 
uint32_t blinkNow;

String inputString = "";

uint8_t state = S_IDLE;

void setup() {

  inputString.reserve(200);

  Serial.begin(SERIAL_SPEED);
  HARDWARE.begin(SERIAL_SPEED);

  pinMode(btnPin1, INPUT);
  pinMode(btnPin2, INPUT);
  pinMode(LEDPin, OUTPUT);

}

void loop() {
  switch (state) {
  case S_IDLE:
    blinkNow = millis();
    if ((blinkNow - blinkDelta) > blinkInterval) {
      blinkOn = !blinkOn;
      digitalWrite(LEDPin, blinkOn);
      blinkDelta = blinkNow;
    }

    // build buffer that gets sent when polled.
    // first, collect if buttons were pressed between polling.
    // does not send the state of the buttons at time of polling.
    btn_state1 = digitalRead(btnPin1);
    btn_state2 = digitalRead(btnPin2);
    dataBuf[0] = '$'; 
    if (btn_state1 == 1) {dataBuf[1] = 1;} 
    if (btn_state2 == 1) {dataBuf[2] = 1;}

    // then get state of the joysticks
    joy1PinVal= map(analogRead(joy1Pin),0,1024,1024,0);
    joy2PinVal= map(analogRead(joy2Pin),0,1024,1024,0);
    joy3PinVal= map(analogRead(joy3Pin),0,1024,1024,0);
    joy4PinVal= map(analogRead(joy4Pin),0,1024,1024,0);
    dataBuf[3] = highByte(joy1PinVal);  dataBuf[4] = lowByte(joy1PinVal);
    dataBuf[5] = highByte(joy2PinVal);  dataBuf[6] = lowByte(joy2PinVal);
    dataBuf[7] = highByte(joy3PinVal);  dataBuf[8] = lowByte(joy3PinVal);
    dataBuf[9] = highByte(joy4PinVal); dataBuf[10] = lowByte(joy4PinVal);
    dataBuf[11] = '\n';

    break;
  case S_POLLED:
    // do your job - send data to master
    HARDWARE.write(dataBuf, MEM_LEN);
    // Serial.write(dataBuf, MEM_LEN);

    dataBuf[1] = LOW; // reset these after polling
    dataBuf[2] = LOW; 
    state = S_IDLE; 

    break;
  case S_REPORT_STATUS:
    // talk to the USB serial
    Serial.printf("buttons %d %d\n", dataBuf[1], dataBuf[2]);
    Serial.printf("joy %d %d %d %d\n",
		  joy1PinVal, joy2PinVal, joy3PinVal, joy4PinVal);
    state = S_IDLE;

    break;
  default:
    Serial.println ("Unknown state") ;
    break;
  }

}

void serialEvent1() {
  uint8_t count = 0;
  while (HARDWARE.available()) {
    char inChar = (char)HARDWARE.read();
    inputString += inChar;
    // user sent command: "Q\n"
    if (inChar == '\n' && count == 1 && inputString[0] == 'Q') {
      state = S_POLLED;
    }
    count++;
  }
  inputString = "";
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      handle_cmd();
    }
  }
}

void handle_cmd() {
  inputString.trim(); // removes beginning and ending white spaces
  int idx = inputString.indexOf(' ');   
  String cmd = inputString.substring(0, idx); 
  String value = inputString.substring(idx + 1);

  if ((cmd.length() > 0)) {
    if (cmd.equals("probe_device")) {
      Serial.println("joy stick");
    }
    if (cmd.equals("idle")) {
      Serial.println(cmd);
      state = S_IDLE;
    }
    if (cmd.equals("report_status")) {
      Serial.println(cmd);
      state = S_REPORT_STATUS;
    }
    if (cmd.equals("poll")) {
      Serial.println(cmd);
      state = S_POLLED;
    }
    inputString = "";
  }
}
