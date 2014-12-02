/*

NeoPixels

*/

#include <ArduinoRobot.h>
#include <Adafruit_NeoPixel.h>

#define PIN_NEO TKD1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(9, PIN_NEO, NEO_GRB + NEO_KHZ800);

void setup() {
  // initialize the robot
  Robot.begin();
  
  // initialize the screen
  Robot.beginTFT();
  Robot.beginSD();
  Robot.drawBMP("intro.bmp",0,0);
  
  Robot.textSize(1);
  
  

  strip.begin();
  strip.show();
  
}
void loop(){
  // Some example procedures showing how to display to the pixels:
  
  
  textShow("colorWipe: red"); 
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  textHide("colorWipe: red"); 
  
  textShow("colorWipe: green");
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  textHide("colorWipe: green"); 
  
  textShow("colorWipe: blue"); 
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
  textHide("colorWipe: blue"); 
  
  // Send a theater pixel chase in...
  textShow("theaterChase: white");
  theaterChase(strip.Color(127, 127, 127), 50); // White
  textHide("theaterChase: white");
  
  textShow("theaterChase: red");
  theaterChase(strip.Color(127,   0,   0), 50); // Red
  textHide("theaterChase: red");
  
  textShow("theaterChase: blue");
  theaterChase(strip.Color(  0,   0, 127), 50); // Blue
  textHide("theaterChase: blue");

  textShow("rainbow"); 
  rainbow(20);
  textHide("rainbow"); 
  
  textShow("rainbowCycle");
  rainbowCycle(20);
  textHide("rainbowCycle"); 
  
  textShow("theaterChaseRainbow"); 
  theaterChaseRainbow(50);
  textHide("theaterChaseRainbow");

}

void textShow(const char * text)
{
  Robot.stroke(0,0,0);
  Robot.text(text, 10, 10); 
}

void textHide(const char * text)
{
  Robot.stroke(255,255,255);
  Robot.text(text, 10, 10); 
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

