
#include <avr/io.h>
#include <avr/interrupt.h>


#define PIN_INT_LEFT 2
#define PIN_DEBUG 13

volatile byte state = LOW;

// Install the interrupt routine.
ISR(INT0_vect) {
  // check the value again - since it takes some time to
  // activate the interrupt routine, we get a clear signal.
  state = digitalRead(PIN_INT_LEFT);
  digitalWrite(PIN_DEBUG, state);
}

void setup() 
{                
  Serial.begin(57600);
  pinMode(PIN_INT_LEFT, INPUT );
  pinMode(PIN_DEBUG, OUTPUT);
  //attachInterrupt(0, blink, CHANGE);
  
  state = digitalRead(PIN_INT_LEFT);
  
    Serial.println("Processing initialization");
  EICRA |= (1 << ISC00);    // set INT0 to trigger on ANY logic change
  EIMSK |= (1 << INT0);     // Turns on INT0
  Serial.println("Finished initialization");
  
}


void loop()
{
  
}

void blink()
{
  state = !state;
  digitalWrite(PIN_DEBUG, state);
}
