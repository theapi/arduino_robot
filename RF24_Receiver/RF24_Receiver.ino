
#include <ArduinoRobot.h>
//#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
//#include "printf.h"

#define PIN_CE  TKD4
#define PIN_CSN TKD3
 

byte addresses[][6] = {"1Node","2Node"};

RF24 radio(PIN_CE, PIN_CSN);

void setup() 
{
  //Serial.begin(57600);
  //printf_begin();
  //printf("\n\r RF24_Receiver \n\r");
  
  // initialize the robot
  Robot.begin();

  // initialize the screen
  Robot.beginTFT();
  
  // Setup and configure rf radio
  radio.begin(); // Start up the radio
  radio.setPayloadSize(2);                // Only two byte payload gets sent (int on arduino) (short on 32bit rpi)
  radio.setAutoAck(1); // Ensure autoACK is enabled
  radio.setRetries(15,15); // Max delay between retries & number of retries
  radio.openWritingPipe(addresses[1]); // "ping back"
  radio.openReadingPipe(1,addresses[0]);
  radio.startListening(); // Start listening
  
  //radio.printDetails();                   // Dump the configuration of the rf unit for debugging
}

void loop(void)
{
  
  // Check for a message from the controller
  if (radio.available()) {
    // Get the payload
    /*
    unsigned long got_time; 
    while (radio.available()) {                                   // While there is data ready
        radio.read( &got_time, sizeof(unsigned long) );             // Get the payload
      } 
    printf("Got: %lu \n\r", got_time);
    */
    
    
    int got_val;                 
    radio.read( &got_val, sizeof(int) );    
      
    if (got_val > 0) {
      // Do something...
      //printf("Got: %d \n\r", got_val);
      Robot.debugPrint(got_val);
    }
    
  }
}
