
#include <ArduinoRobot.h>
//#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

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

const int rx_buf_size = 10;
byte rx_buf[rx_buf_size];
byte rx_buf_index = 0;

char screen_radio_msg[20];

void setup() 
{
  //Serial.begin(57600);
  // initialize the robot
  Robot.begin();
  
  // Restart internal serial comms at a faster rate.
  Serial1.end();
  Serial1.begin(57600);
  
  // initialize the screen
  Robot.beginTFT();
  
  // Black screen
  Robot.background(0,0,0);

  Robot.stroke(255,255,255);
 
  //Serial.println(" BEGIN ");
  
  //Robot.text("BEGIN", 10, 10);
}

void loop()
{
  
  //processMotorMessage();
  
  
  getMotorStatus();
  
  delay(1000);
}


void getMotorStatus()
{
  Serial1.write('s');
  Serial1.flush(); // wait for the message to be sent
  
  processMotorMessage(); 
    
}

void processMotorMessage()
{     
  byte msg_available = 0;
    
  while(Serial1.available()) {
    msg_available = 0;
    rx_buf[rx_buf_index++] = Serial1.read();
    if (rx_buf_index == rx_buf_size) {
      // Buffer full so entire message received
      // NB may lose data if constantly streaming in.
      // But we get the last good data sent.
      msg_available = 1;
      
      // Reset to start a new message
      rx_buf_index = 0;
    }
  }
    
  if (msg_available) {
    // Get the message out of the buffer now.
    
    int msg_id = (rx_buf[2] << 8) | rx_buf[3];
    int a = (rx_buf[4] << 8) | rx_buf[5];
    int b = (rx_buf[6] << 8) | rx_buf[7];
    /*
    Serial.print("GOT: ");
    Serial.print((char)rx_buf[0]);
    Serial.print(" : ");
    Serial.print(a);
    Serial.print(" : ");
    Serial.println(b);
    */
    // Clear the old
    Robot.stroke(0,0,0);
    Robot.text(screen_radio_msg, 0, 0);
    
    sprintf(screen_radio_msg, "%c, %c, %d, %d, %d", rx_buf[0], rx_buf[1], msg_id, a, b);
    Robot.stroke(255,255,255);
    Robot.text(screen_radio_msg, 0, 0);
  }

}
