/*
 Battery indicator on the TFT screen.
 */

#include <ArduinoRobot.h>

int value;
int value_old;

void setup() {
  // initialize the robot
  Robot.begin();
  
  // initialize the screen
  Robot.beginTFT();
  
  // Black screen
  Robot.background(0,0,0);

  batteryDraw();
  batteryUpdate();
}
void loop()
{
  batteryUpdate();
  delay(3000);
}

void batteryDraw()
{
  Robot.stroke(255, 255, 255); 
  // Body
  Robot.line(115, 0, 115, 6);
  Robot.line(115, 0, 125, 0); 
  Robot.line(115, 5, 125, 5);
  // Tip
  Robot.line(125, 1, 127, 1);
  Robot.line(125, 4, 127, 4);
  Robot.line(127, 1, 127, 5);
}

void batteryUpdate()
{
  Robot.noStroke();
  value_old = value;
  value = readVcc();
  int level = map(value, 4400, 5000, 1, 11);
  if (level < 3) {
    Robot.fill(255,0,0); 
  } else if (level < 7) {
    Robot.fill(0,0,255); 
  } else {
    Robot.fill(0,255,0); 
  }
  Robot.rect(116, 1, level, 4);
  
  // Large number center screen 
  // @todo: remove
  // Clear
  Robot.stroke(0,0,0);
  Robot.textSize(3);
  Robot.text(value_old, 15, 50); 
  // Write
  Robot.stroke(0,255,0);
  Robot.textSize(3);
  Robot.text(value, 15, 50); 
  
  Robot.textSize(2);
  Robot.text("mV", 92, 58); 
}

long readVcc() 
{
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
