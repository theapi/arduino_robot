
#include "Arduino.h"
#include "RobotBattery.h"


RobotBattery::RobotBattery() {}

/**
 * Robot.beginTFT(); must have been called before this.
 */
void RobotBattery::beginIcon(uint8_t r, uint8_t g, uint8_t b)
{
  _r = r;
  _g = g;
  _b = b;
  iconEnable();
  iconDraw();
}

void RobotBattery::iconEnable()
{
  _icon = 1;
}

void RobotBattery::iconDisable()
{
  _icon = 0;
}

uint8_t RobotBattery::iconEnabled()
{
  return _icon;
}

void RobotBattery::iconDraw()
{
  // Draw the battery outline
  Robot.stroke(_r, _g, _b); 
  // Body
  Robot.line(115, 0, 115, 6);
  Robot.line(115, 0, 125, 0); 
  Robot.line(115, 5, 125, 5);
  // Tip
  Robot.line(125, 1, 127, 1);
  Robot.line(125, 4, 127, 4);
  Robot.line(127, 1, 127, 5);
}


int RobotBattery::update()
{
  value_old = value;
  value = readVcc();
  
  if (value == value_old) {
    // No need to redraw the same value to the screen.
    return value;
  }

  if (iconEnabled()) {
    Robot.noStroke();
    int level = map(constrain(value, 4400, 5000), 4400, 5000, 1, 11);
    if (level < 3) {
      Robot.fill(255,0,0); 
    } else if (level < 7) {
      Robot.fill(255,255,0); 
    } else {
      Robot.fill(0,255,0); 
    }
    Robot.rect(116, 1, level, 4);
  }
  
  return value;
}

int RobotBattery::readVcc() 
{
  byte original_admux = ADMUX;
  byte original_adcsra = ADCSRA;
  byte original_adcsrb = ADCSRB;
  
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
 
  // prescaler of 32, speed's better than accuracy.
  ADCSRA |= _BV(ADPS2) | _BV(ADPS0);
 
  // Here's the blocking part, 
  // but it's not a bad idea as you don't want analogRead() messing the readings
 
  // Wait for Vref to settle
  delayMicroseconds(500);
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring
   
  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both
  long result = (high<<8) | low;
  
  // return registries to previous settings, just in case.
  ADMUX = original_admux;
  ADCSRA = original_adcsra;
  ADCSRB = original_adcsrb;
 
  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return (int) result; // Vcc in millivolts
}
