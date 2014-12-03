/*
  RobotBattery.h - Library for reading the robot's battery level.
  
  Draw a small battery icon with level indication with updateIcon()
  
  Created by Peter Clarke December 3, 2014.
*/
#ifndef RobotBattery_h
#define RobotBattery_h

#include "Arduino.h"
#include <ArduinoRobot.h>

class RobotBattery
{
  public:
    RobotBattery();
    
    // Robot.beginTFT(); must be called before begin()
    void beginIcon(uint8_t r=0, uint8_t g=0, uint8_t b=0);
    // Get a new reading
    int update();
    // Read the ADC
    int readVcc();
    // Disable showing the icon
    void iconDisable();
    // Enable drawing the icon
    void iconEnable();
    // Draw the icon
    void iconDraw();
    // Whether the icon should be drawn
    uint8_t iconEnabled();
    
  private:
     // The current value
    int value;
    // The previous value
    int value_old;
    // Whether to show the icon
    uint8_t _icon;
    uint8_t _r;
    uint8_t _g;
    uint8_t _b;
    

};
#endif

