/*
 Battery indicator on the TFT screen.
 */

#include <ArduinoRobot.h>
#include "RobotBattery.h"

RobotBattery battery = RobotBattery();
int bat_val_prev = 0;

void setup() {
  
  // initialize the robot
  Robot.begin();
  
  // initialize the screen
  Robot.beginTFT();
  
  // Black screen
  Robot.background(0,0,0);

  // Draw the battery icon with white edges.
  battery.beginIcon(255, 255, 255);

}
void loop()
{

  int bat_val = battery.update();
  if (bat_val != bat_val_prev) {
    
    // Large number center screen 
    // Clear
    Robot.stroke(0,0,0);
    Robot.textSize(3);
    Robot.text(bat_val_prev, 15, 50); 
    // Write
    Robot.stroke(0,255,0);
    Robot.textSize(3);
    Robot.text(bat_val, 15, 50); 
    
    Robot.textSize(2);
    Robot.text("mV", 92, 58); 
    
    bat_val_prev = bat_val;
  }
 
  delay(2000);
}

