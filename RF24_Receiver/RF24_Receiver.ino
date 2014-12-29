
/***** NB FOR MOTOR BOARD ***/

#define DEVICE_ID 'D'
 
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
#include <Nrf24Payload.h>

#define PIN_CE  SCL
#define PIN_CSN SDA
 
// The address that this node listens on
byte address[6] = RX_ADDRESS;
byte address_base[6] = BASE_ADDRESS;
int ack = 0;

RF24 radio(PIN_CE, PIN_CSN);

Nrf24Payload rx_payload = Nrf24Payload();
uint8_t rx[Nrf24Payload_SIZE];

uint16_t msg_id = 0;

byte tx_buf[10];

int speedL = 0;
int speedR = 0;

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
  radio.setPayloadSize(Nrf24Payload_SIZE); 
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

    radio.read( &rx, Nrf24Payload_SIZE); 
    processPayload();
    
  }

}

void processPayload()
{
  rx_payload.unserialize(rx);
  
  // Create an internal transfer buffer of the relevant data.  
  //@todo provide usful data for the top board
  tx_buf[0] = rx_payload.getType(); 
  tx_buf[1] = rx_payload.getDeviceId();
  tx_buf[2] = (rx_payload.getId() >> 8); 
  tx_buf[3] = rx_payload.getId(); 
  tx_buf[4] = (rx_payload.getA() >> 8); 
  tx_buf[5] = rx_payload.getA(); 
  tx_buf[6] = (rx_payload.getB() >> 8); 
  tx_buf[7] = rx_payload.getA(); 
  
    
  // Proof of concept
  if (rx_payload.getA() == 70) {
    radio.stopListening();
    
    // Prepare the message.
    Nrf24Payload tx_payload = Nrf24Payload();      
    tx_payload.setDeviceId(DEVICE_ID);
    tx_payload.setType('q'); // light command
    tx_payload.setTimestamp(millis());
    tx_payload.setId(msg_id++);
    tx_payload.setA(99);
    
    uint8_t tx_buffer[Nrf24Payload_SIZE];
    tx_payload.serialize(tx_buffer);

    //printf("sending %d, %d \n", payload.msg_id, payload.a);    
    if (!radio.write( &tx_buffer, Nrf24Payload_SIZE)) { 
      //printf(" failed.\n\r"); 
    }
    
    radio.startListening(); 
  } 
  
  int c = rx_payload.getC();
  int d = rx_payload.getD();
  
  if (c > 0) {
    // Left motor command
    if (c > 255) {
      // Forward 0 -> 255
      speedL = constrain(c - 255, 0, 255);
    } else {
      // Backward -1 -> -255
      speedL = 1 - constrain(c , 0, 255);
    }
  } else {
    speedL = 0; // stopped
  }
  
  if (d > 0) {
    // Right motor command
    if (d > 255) {
      // Forward 0 -> 255
      speedR = constrain(d - 255, 0, 255);
    } else {
      // Backward -1 -> -255
      speedR = 1 - constrain(d , 0, 255);
    }
  } else {
    speedR = 0; // stopped
  }
  
  RobotMotor.motorsWrite(speedL, speedR);
  
}

