
/***** NB FOR MOTOR BOARD ***/

#define DEVICE_ID 'D';
 
//#define RX_ADDRESS "AAAAA"
//#define RX_ADDRESS "BBBBB"
//#define RX_ADDRESS "CCCCC"
#define RX_ADDRESS "DDDDD"
//#define RX_ADDRESS "EEEEE"
// @todo: hardware defined address

#define BASE_ADDRESS "1BASE"

#include <ArduinoRobotMotorBoard.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

#define PIN_CE  SCL
#define PIN_CSN SDA
 
// The address that this node listens on
byte address[6] = RX_ADDRESS;
byte address_base[6] = BASE_ADDRESS;
int ack = 0;

RF24 radio(PIN_CE, PIN_CSN);

/**
 * Be carefull to ensure the struct size is the same as on the Pi.
 * Just having the same size variables is not enough.
 * @see http://www.delorie.com/djgpp/v2faq/faq22_11.html
 */
typedef struct{
  int32_t timestamp;
  uint16_t msg_id;
  uint16_t vcc;
  uint16_t a;
  uint16_t b;
  uint16_t c;
  uint16_t d;
  uint8_t type;
  uint8_t device_id;
  int8_t y;
  int8_t z;
}
payload_t;
payload_t payload;


uint16_t msg_id = 0;

byte tx_buf[10];

void setup() 
{

  tx_buf[0] = '-';
  tx_buf[1] = DEVICE_ID;
    
  //pinMode(SS, OUTPUT);
    
  //delay(2000);
  //printf_begin();
  //printf("\n\r RF24_Receiver on address: %s \n\r", RX_ADDRESS);
 
  // initialize the robot
  RobotMotor.begin();
  
  // Restart internal serial comms at a faster rate.
  Serial1.end();
  Serial1.begin(57600);
  
  // Setup and configure rf radio
  radio.begin(); // Start up the radio
  radio.setPayloadSize(sizeof(payload_t)); 
  radio.setAutoAck(1); // Ensure autoACK is enabled
  radio.setRetries(0,15); // Max delay between retries & number of retries
  // Allow optional ack payloads
  radio.enableAckPayload();
  
  // Pipe for talking to the base
  radio.openWritingPipe(address_base);
  
  // Pipe for listening to the base
  radio.openReadingPipe(1, address);
  
  // Start listening
  radio.startListening(); 
  
  //radio.printDetails(); 
  

  
 // Robot.debugPrint(sizeof(payload));
}

void loop(void)
{
  
  RobotMotor.parseCommand();
  RobotMotor.process();
  
  if (Serial1.available()) {
    byte c = Serial1.read();
    if (c == 's') {
      Serial1.write(tx_buf, 10);
      Serial1.flush(); // wait for the message to be sent
    }
  }
  
  // Check for a message from the controller
  if (radio.available()) {
    // Get the payload
    // Create the ack payload for the NEXT message.
    radio.writeAckPayload(1, &ack, sizeof(ack));
    ack++; 

    radio.read( &payload, sizeof(payload));  
    processPayload();
    
  }

}

void processPayload()
{
  // Create a transfer buffer of the relevant data.  
  tx_buf[0] = payload.type; 
  tx_buf[1] = payload.device_id;
  tx_buf[2] = (payload.msg_id >> 8); 
  tx_buf[3] = payload.msg_id; 
  tx_buf[4] = (payload.a >> 8); 
  tx_buf[5] = payload.a; 
  tx_buf[6] = (payload.b >> 8); 
  tx_buf[7] = payload.b; 
  //Serial1.write(tx_buf, 10);
  
  
  //uint8_t *bytePtr = (uint8_t*)&payload;
  //Serial1.write(bytePtr, sizeof(payload));
 /* 
    printf ("Got: %c %c %ld %d %d %d %d %d %d %d %d \n",
    payload.device_id,
    payload.type,
    payload.timestamp,
    payload.msg_id,
    payload.vcc,
    payload.a,
    payload.b,
    payload.c,
    payload.d,
    payload.y,
    payload.z);
    */
  // Proof of concept
  if (payload.a == 70) {
    radio.stopListening();
    
    // Prepare the message.
    payload_t payload;
    payload.device_id = DEVICE_ID;
    payload.type = 'q';
    payload.timestamp = millis();
    payload.msg_id = msg_id;
    payload.vcc = 0; //@TODO vcc
    payload.a = 99;
    payload.b = 0;
    payload.c = 0;
    payload.d = 0;
    payload.y = 0;
    payload.z = 0;
    
    //printf("sending %d, %d \n", payload.msg_id, payload.a);    
    if (!radio.write( &payload, sizeof(payload))) { 
      //printf(" failed.\n\r"); 
    }
    
    msg_id++; // Let it overflow
    radio.startListening(); 
  }
}

