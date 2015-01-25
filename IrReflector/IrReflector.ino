 

#define PIN_INT_LEFT 2
#define PIN_MOTOR_LEFT 11
#define PIN_POT A5
#define PIN_DEBUG 4

long pot_read = 0;
long pot_interval = 100;

volatile byte state = LOW;


void setup() 
{                
  Serial.begin(57600);
  pinMode(PIN_INT_LEFT, INPUT );
  pinMode(PIN_MOTOR_LEFT, OUTPUT);
  pinMode(PIN_DEBUG, OUTPUT);
  attachInterrupt(0, blink, CHANGE);
  
  state = digitalRead(PIN_INT_LEFT);
  
  analogWrite(PIN_MOTOR_LEFT, 128);
  
}


void loop()
{
  
  unsigned long now = millis();
  if (now - pot_read >= pot_interval) {
    pot_read = now;
    int pot_val = analogRead(PIN_POT);
    byte pwm_motor_left = map(pot_val, 0, 1023, 0, 254);
    analogWrite(PIN_MOTOR_LEFT, pwm_motor_left);
  }
  
}

void blink()
{
  // check the value again - since it takes some time to
  // activate the interrupt routine, we get a clear signal.
  state = digitalRead(PIN_INT_LEFT);
  digitalWrite(PIN_DEBUG, state);
}
