# Joystick controller

## INTRODUCTION
![Joystick](http://i.imgur.com/1tFL8Y6.jpg) 

I wanted a simple joystick controller. There are other [examples](http://store.curiousinventor.com/guides/PS2/) of getting an arduino to playstation controllers but at least for the controller I bought, there was a useless deadband when the sticks were centered that made it useless. So I ordered a couple [joysticks](http://www.robotshop.com/en/2-axis-joystick.html) and built my own.

## WIRING
The joystick modules are composed of two potentiometers, and reading them was as simple as the typical [AnalogRead](https://www.arduino.cc/en/Tutorial/ReadAnalogVoltage) circuit. Similarly I used two push buttons with a pulldown resistor [shown here](https://www.arduino.cc/en/tutorial/button). I have a custom circuit board that I can drop a teensy onto, the final assembly is [here](http://i.imgur.com/AM6NEAo.jpg), a view of the underside of the custom pcb that let's me do wirewrap is [here](http://i.imgur.com/KBIptDZ.jpg). Four wires come into the joystick - Serial tx, rx, +3.2v and gnd. 

## CONSTRUCTION
* Step 1: build [laser](https://www.reddit.com/r/lasercutting/comments/55f0kb/meet_my_300w_co2_laser/) that cuts sheet metal and wood;
* Step 2: design [[files](https://github.com/owhite/arduino/blob/master/joystick/docs/joystick.pdf)] [[dxf](https://github.com/owhite/arduino/blob/master/joystick/docs/joystick.dxf)];
* Step 3: print parts, and assemble;

## CODE
The joystick works using two microcontollers - the slave microcontroller is embedded in the joystick box, and the master reads data via the serial from the slave. Yes, using two microcontrollers is overkill. I did this because the final system that will use the joystick will have a lot of other input, and I was worried about running out of i/o, so I thought I'd handle data the four potentiometers and two switches and stream them to the master. I also hope make the joystick wireless and would need to run the joystick through a bluetooth serial at some point. 

Code for the slave is [here](https://raw.githubusercontent.com/owhite/arduino/master/joystick/slave/slave.ino). Nothing complicated, it uses a state machine and sends the data out the teensy's hardware serial. The teensy reads the status of the pushbuttons, the joysticks, loads up a data buffer. It then loops until it receives the command "$\n" from the master and then sends out the data.

Here's a snippet:
```
    #define MEM_LEN 12
    uint8_t dataBuf[MEM_LEN]; 
    .
    . 
    .
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
```

dataBuf just stores bytes, and the data from each joystick is a full int, so each int is decomposed into two bytes and stuff into an array element of dataBuf.

[This](https://raw.githubusercontent.com/owhite/arduino/master/joystick/master/master.ino) is the code for the master. Also nothing complicated. After it receives a chunk of data from the serial it goes to the call:

```
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
```
Where the function word() helps recompose the two bytes into a 16-bit integer.  

Enjoy.
