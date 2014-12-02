/*
 LCD Debug Print
 
 Use the Robot's library function debugPrint() to
 quickly send a sensor reading to the robot's creen.
 
 Circuit:
 * Arduino Robot
 
 created 1 May 2013
 by X. Yang
 modified 12 May 2013
 by D. Cuartielles
 
 This example is in the public domain
 */

#include <ArduinoRobot.h>

int value;
int value_old;

void setup() {
  // initialize the robot
  Robot.begin();
  

  // initialize the screen
  Robot.beginTFT();
  
  Robot.beginSD();
  
    //The image is pulled from sd card
  Robot.drawBMP("intro.bmp",0,0);
  
}
void loop(){
  value_old = value;
  
  // read a value
  value = readVcc();


  // Clear
  Robot.stroke(255,255,255);
  Robot.textSize(3);
  Robot.text(value_old, 15, 50); 
  // Write
  Robot.stroke(0,255,0);
  Robot.textSize(3);
  Robot.text(value, 15, 50); 
  
  Robot.textSize(2);
  Robot.text("mV", 92, 58); 


  delay(3000);
}

long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  
 
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring
 
  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both
 
  long result = (high<<8) | low;
 
  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}
