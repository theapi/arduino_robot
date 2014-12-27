/*
----------------------------------------------------

           PhotoR     10K
 +5    o---/\/\/--.--/\/\/---o GND
                  |
 Pin 0 o-----------

----------------------------------------------------
*/

#include <PID_v1.h> // See PID_Frontend_ArduinoSampleCode


#define PIN_INT_LEFT 2
#define PIN_MOTOR_LEFT 11
#define PIN_POT A5

//Define Variables we'll be connecting to
double Setpoint, Input, Output;
int inputPin=0, outputPin=3;

//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint, 1.0, 0.5, 0.0, DIRECT);
unsigned long serialTime; //this will help us know when to talk with processing

int count =0;
long sample_time = 0;
long sample_interval = 10; // 100 Hz sample rate
byte sample_previous = HIGH;
unsigned long ms_per_int = 0; // How long since the last beam interrupt

int speed_requested = 0;
byte pwm_motor_left = 0;
int speed_motor_left = 0;

//int speed_current = 0;
//int num_spokes = 5;

void setup()
{
  Serial.begin(57600);
  pinMode(PIN_INT_LEFT, INPUT );

  
  speed_requested = analogRead(PIN_POT) / 4;
  
  
  //initialize the variables we're linked to
  Input = 0;
  Setpoint = 500;

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(89, 255);
  myPID.SetSampleTime(sample_interval-1); // ensure a pid compute for every speed sample

  //Serial.println("Begin");
      
}

void loop()
{
  unsigned long now = millis();
  if (now - sample_time >= sample_interval) {
    sample_time = now;
    byte sample = digitalRead(PIN_INT_LEFT);
    if (sample != sample_previous) {
      sample_previous = sample;
      if (sample == LOW) {
        // Just seen a black bit.
        ms_per_int = count * sample_interval;
        
        if (ms_per_int > 700) {
          Input = 0;
        } else {
          
          // Switch low to high
          Input = map(ms_per_int, 0, 700, 700, 0);
          //speed_motor_left = map(ms_per_int, 0, 1023, 1023, 0);
          
          //speed_motor_left = ms_per_int;
          //Input = speed_motor_left;
          //Serial.print(" : "); Serial.print(speed_motor_left);
          //Serial.print(" - "); Serial.println(Input);
          
          count = 0;
        }
      } else {
        // Check for stopped/too slow
        ms_per_int = count * sample_interval;
        if (ms_per_int > 700) {
          count = 0;
          Input = 0;
        }
      }
      
    } else {
      
      // Nah not right yet for slow
      
      // No change in light
      // Check for stopped/too slow
      
      ms_per_int = count * sample_interval;
      if (ms_per_int > 700) {
        //count = 0;
        //Input = 0;
      }
      
      
      if (count > 5) {
      //Input = 0;
        
      }
      
      if (count > 0) {
      //Input = Input / count;
      }
    }
    
    count++; // Up the sample count
    
    // Do the computation now
    myPID.Compute();
    
    
    pwm_motor_left = Output;
    //pwm_motor_left = constrain(pwm_motor_left, 100, 180);
    if (pwm_motor_left < 90) pwm_motor_left = 0;
    analogWrite(PIN_MOTOR_LEFT, pwm_motor_left);
       
    
    Setpoint = analogRead(PIN_POT); // NB motor can't go faster than 700 :(
    if (Setpoint < 90) {
      // Can't go slower than this
      Setpoint = 0; 
    }
    
    
  }
  

  //send-receive with processing if it's time
  if (millis()>serialTime) {
    SerialReceive();
    SerialSend();
    serialTime+=250;
  }
  

}


/********************************************
 * Serial Communication functions / helpers
 ********************************************/


union {                // This Data structure lets
  byte asBytes[24];    // us take the byte array
  float asFloat[6];    // sent from processing and
}                      // easily convert it to a
foo;                   // float array



// getting float values from processing into the arduino
// was no small task.  the way this program does it is
// as follows:
//  * a float takes up 4 bytes.  in processing, convert
//    the array of floats we want to send, into an array
//    of bytes.
//  * send the bytes to the arduino
//  * use a data structure known as a union to convert
//    the array of bytes back into an array of floats

//  the bytes coming from the arduino follow the following
//  format:
//  0: 0=Manual, 1=Auto, else = ? error ?
//  1: 0=Direct, 1=Reverse, else = ? error ?
//  2-5: float setpoint
//  6-9: float input
//  10-13: float output  
//  14-17: float P_Param
//  18-21: float I_Param
//  22-245: float D_Param
void SerialReceive()
{

  // read the bytes sent from Processing
  int index=0;
  byte Auto_Man = -1;
  byte Direct_Reverse = -1;
  while(Serial.available()&&index<26)
  {
    if(index==0) Auto_Man = Serial.read();
    else if(index==1) Direct_Reverse = Serial.read();
    else foo.asBytes[index-2] = Serial.read();
    index++;
  } 
  
  // if the information we got was in the correct format, 
  // read it into the system
  if(index==26  && (Auto_Man==0 || Auto_Man==1)&& (Direct_Reverse==0 || Direct_Reverse==1))
  {
    Setpoint=double(foo.asFloat[0]);
    //Input=double(foo.asFloat[1]);       // * the user has the ability to send the 
                                          //   value of "Input"  in most cases (as 
                                          //   in this one) this is not needed.
    if(Auto_Man==0)                       // * only change the output if we are in 
    {                                     //   manual mode.  otherwise we'll get an
      Output=double(foo.asFloat[2]);      //   output blip, then the controller will 
    }                                     //   overwrite.
    
    double p, i, d;                       // * read in and set the controller tunings
    p = double(foo.asFloat[3]);           //
    i = double(foo.asFloat[4]);           //
    d = double(foo.asFloat[5]);           //
    myPID.SetTunings(p, i, d);            //
    
    if(Auto_Man==0) myPID.SetMode(MANUAL);// * set the controller mode
    else myPID.SetMode(AUTOMATIC);             //
    
    if(Direct_Reverse==0) myPID.SetControllerDirection(DIRECT);// * set the controller Direction
    else myPID.SetControllerDirection(REVERSE);          //
  }
  Serial.flush();                         // * clear any random data from the serial buffer
}

// unlike our tiny microprocessor, the processing ap
// has no problem converting strings into floats, so
// we can just send strings.  much easier than getting
// floats from processing to here no?
void SerialSend()
{
  Serial.print("PID ");
  Serial.print(Setpoint);   
  Serial.print(" ");
  Serial.print(Input);   
  Serial.print(" ");
  Serial.print(Output);   
  Serial.print(" ");
  Serial.print(myPID.GetKp());   
  Serial.print(" ");
  Serial.print(myPID.GetKi());   
  Serial.print(" ");
  Serial.print(myPID.GetKd());   
  Serial.print(" ");
  if(myPID.GetMode()==AUTOMATIC) Serial.print("Automatic");
  else Serial.print("Manual");  
  Serial.print(" ");
  if(myPID.GetDirection()==DIRECT) Serial.println("Direct");
  else Serial.println("Reverse");
}

