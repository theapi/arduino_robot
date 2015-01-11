

#define PIN_INT_LEFT 2
#define PIN_DEBUG 13

volatile byte state = LOW;


void setup() 
{                
  Serial.begin(57600);
  pinMode(PIN_INT_LEFT, INPUT );
  pinMode(PIN_DEBUG, OUTPUT);
  attachInterrupt(0, blink, CHANGE);
  
  state = digitalRead(PIN_INT_LEFT);
  
}


void loop()
{
  
}

void blink()
{
  // check the value again - since it takes some time to
  // activate the interrupt routine, we get a clear signal.
  state = digitalRead(PIN_INT_LEFT);
  digitalWrite(PIN_DEBUG, state);
}
