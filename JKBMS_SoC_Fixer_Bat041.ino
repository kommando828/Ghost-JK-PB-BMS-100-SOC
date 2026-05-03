//JKBMS SoC Fixer
//Hardware: M5 Atom Lite (C008) with RS-485 base (A131). Also works with M5 AtomS3

//required software components
//Adafruit_Neopixel library https://github.com/adafruit/Adafruit_NeoPixel (available from the arduino library manager)
//M5Stack board support library https://github.com/m5stack
//See https://docs.m5stack.com/en/arduino/arduino_board for details
//note - the M5 arduino library is not necessary (just the board support package)

//Wiring Diagram
//Asuming you are using the M5 RS-485 Base (M5 A131):
//Cut a cat5 cable in half and connect:
// pin 1 (orange conductor) to A on the RS485 terminal
// pin 2 (orange/white conductor) to B on the rs485 terminal
// pin 3 (green) wire to ground
// note - there are internal pulldown resistors in the A131 module so a dedicated ground connection is not necessary
//NOTE: the M5 A131 is not isolated. If you need bus isolation you can use an isolated RS485 module (ie, M5 U094) and update the pins accordingly 


#include <Adafruit_NeoPixel.h>
#include "JKModbusSlave.h"

#define ADDRESS 4 //battery address to spoof. There should be no physical battery at this address
                   

//Atom LITE
#ifdef ARDUINO_M5STACK_ATOM
  #define BUTTON 39
  #define RGB_PIN 27    //serial pin for atom LED
  #define RX 22         //RX pin for RS485 coms
  #define TX 19         //TX pin for RS485 coms
  #define INVERT false  //allows to invert RS485 coms polarity
#endif

//AtomS3 LITE
#ifdef ARDUINO_M5STACK_ATOMS3
  #define BUTTON 41
  #define RGB_PIN 35    //serial pin for atom LED
  #define RX 5          //RX pin for RS485 coms
  #define TX 6          //TX pin for RS485 coms
  #define INVERT false  //allows to invert RS485 coms polarity
#endif

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, RGB_PIN, NEO_GRB + NEO_KHZ800);
JKModbusSlave modbus(Serial2);

bool JKBMS_PRINT_DEBUG = false;      //print buss traffic in a human readable form
bool JKBMS_PRINT_PASSTHROUGH = true; //pass through buss traffic (allows monitoring by solar assistant or the JKBMS windows software). 
                                     //note -- the spoofed responses are NOT printed in this mode (only in debug)
                                     //  this means that the spoofed battery will not show up in the monitoring tool
                                     //note -- it probably does not make sense to use this and print_debug at the same time
bool JKBMS_LISTEN_ONLY = false;      //prevents board from transmitting over RS485 (only prints response to terminal--useful when combined with print_debug)


void setup() {
    pixels.begin();           // Init the NeoPixel library for the status LED
    Serial.begin(115200);     // initialize usb serial communication at 115200 baud
    pinMode(BUTTON, INPUT);
    if(digitalRead(BUTTON) == LOW){  //if button is held during boot, switch to debug mode and print RS485 traffic in human readable mode
      JKBMS_PRINT_DEBUG = true;
      JKBMS_PRINT_PASSTHROUGH = false;
      JKBMS_LISTEN_ONLY = false;
      pixels.setPixelColor(0, 0xff0000);  //blink status led red for 1s as confirmation that debug mode is active
      pixels.show();
      delay(1000); 
    }
    modbus.begin(ADDRESS, 115200, SERIAL_8N1, RX, TX, INVERT, JKBMS_LISTEN_ONLY, JKBMS_PRINT_DEBUG, JKBMS_PRINT_PASSTHROUGH); //initialize modbus coms 

}

void loop() {
    //main polling loop 
    //library is configured to blink the following status colors:
    //0x001100 (dim green): library is active but no correctly addressed traffic detected
    //0x0000ff (bright blue): valid address detected, packet decoded correctly
    //0xff0000 (bright red): valid address detected, packet not decoded correctly

    //in normal operation, the LED should idle dim green and flash blue when it is responding to the BMS master
    //if everything is working you should see 2 quick blue flashes every ~10 seconds
    int16_t response = modbus.poll();  
    switch(response){
      case RESPONSE_NO_DATA: //no packet received, display dim green
          pixels.setPixelColor(0, 0x001100);
          pixels.show();
          break;
      case RESPONSE_NOT_MY_ADDRESS: //packet received but not addressed to us, display dim green
          pixels.setPixelColor(0, 0x001100);
          pixels.show();
          break;
      case RESPONSE_ERROR: //error in decoding packet -- flash red
          pixels.setPixelColor(0, 0xff0000);
          delay(100); //blink red for 100ms
          pixels.show();
          break;
      case 0x1E: //frame1 request packet received and decoded successfully -- flash blue
          pixels.setPixelColor(0, 0x0000ff);
          pixels.show();
          delay(10); //blink blue for 10ms
          break;
     case 0x20: //frame2 request packet received and decoded successfully -- flash blue
          pixels.setPixelColor(0, 0x0000ff);
          pixels.show();
          delay(10); //blink blue for 10ms
          break;
     case 0x1C: //frame3 request packet received and decoded successfully -- flash blue
          pixels.setPixelColor(0, 0x0000ff);
          pixels.show();
          delay(10); //blink blue for 10ms
          break;
      default: //invalid frame type -- flash red
          pixels.setPixelColor(0, 0xff0000);
          pixels.show();
          delay(100); //blink red for 100ms
          break;
    }
}
