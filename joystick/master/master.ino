// this is on the controller receiving data from the joystick slave. 

#define SERIAL_SPEED   115200

#define S_IDLE             1
#define S_POLL_JOYSTICK    2
#define S_GET_RESULT       3
#define S_ONE_POLL         4
#define S_REPORT_JOYSTICK  5

#define JOY Serial1
#define MEM_LEN 12
uint8_t dataBuf[MEM_LEN]; 

uint8_t LEDPin = 13;
uint8_t joyCount = 0;

uint8_t btn_state1 = 0;
uint8_t btn_state2 = 0;
uint16_t joy1Val; uint16_t joy2Val;
uint16_t joy3Val; uint16_t joy4Val;

bool gotJoyResult = false;

String inputString = "";

uint8_t state = S_IDLE;

void setup() {

  inputString.reserve(200);

  JOY.begin(SERIAL_SPEED);
  Serial.begin(SERIAL_SPEED);

  pinMode(LEDPin, OUTPUT);

  Serial.println("lets rock");
}

void loop() {
  switch (state) {
  case S_IDLE:
    digitalWrite(LEDPin, LOW);
    break;
  case S_POLL_JOYSTICK:
    digitalWrite(LEDPin, HIGH);
    JOY.write("Q\n");
    state = S_GET_RESULT;
    break;
  case S_GET_RESULT:
    if (gotJoyResult) {
      Serial.printf("buttons %d %d\n", btn_state1, btn_state2);
      Serial.printf("joy %d %d %d %d\n",
		    joy1Val, joy2Val, joy3Val, joy4Val);
    }
    gotJoyResult = false;
    state = S_POLL_JOYSTICK;
    break;
  case S_ONE_POLL:
    digitalWrite(LEDPin, HIGH);
    JOY.write("Q\n");
    gotJoyResult = false;
    state = S_REPORT_JOYSTICK;
    break;
  case S_REPORT_JOYSTICK:
    if (gotJoyResult) {
      Serial.printf("buttons %d %d\n", btn_state1, btn_state2);
      Serial.printf("joy %d %d %d %d\n",
		    joy1Val, joy2Val, joy3Val, joy4Val);
      state = S_IDLE;
    }
    break;
  default:
    Serial.println ("Unknown state") ;
    break;
  }

}

void serialEvent1() {
  while (JOY.available()) {
    uint8_t inByte = JOY.read();
    if (inByte == '$') { joyCount = 0;}
    if (joyCount < MEM_LEN) { dataBuf[joyCount] = inByte; }
    if (inByte == '\n') { parseJoy(); }
    joyCount++;
  }
}

void parseJoy() {
  if(dataBuf[0] == '$' && dataBuf[MEM_LEN - 1] == '\n') {
    btn_state1 = dataBuf[1];
    btn_state2 = dataBuf[2];
    joy1Val = word(dataBuf[3], dataBuf[4]);
    joy2Val = word(dataBuf[5], dataBuf[6]);
    joy3Val = word(dataBuf[7], dataBuf[8]);
    joy4Val = word(dataBuf[9], dataBuf[10]);
    gotJoyResult = true;
  }
  joyCount = 0;
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') { handle_cmd(); }
  }
}

void handle_cmd() {
  inputString.trim(); // removes beginning and ending white spaces
  int idx = inputString.indexOf(' ');   
  String cmd = inputString.substring(0, idx); 
  String value = inputString.substring(idx + 1);

  if ((cmd.length() > 0)) {
    if (cmd.equals("probe_device")) {
      Serial.println("joy master");
    }
    if (cmd.equals("idle")) {
      Serial.println(cmd);
      state = S_IDLE;
    }
    if (cmd.equals("poll_joystick")) {
      state = S_POLL_JOYSTICK;
    }
    if (cmd.equals("one_poll")) {
      Serial.println(cmd);
      state = S_ONE_POLL;
    }
    inputString = "";
  }
}
