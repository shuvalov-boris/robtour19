#ifndef CDRIVECONTROL_H
#define CDRIVECONTROL_H

#include "Arduino.h"
#include "CDriveAxis.h"

// направление движения робота
enum MovementDirection {
  MMD_NONE,  
  MMD_FORWARD, 
  MMD_TURN_LEFT, 
  MMD_TURN_RIGHT, 
  MMD_ROTATE_LEFT, 
  MMD_ROTATE_RIGHT, 
  MMD_BACKWARD,
  MMD_STOP
};

class CDriveControl
{
public: 
  CDriveControl() {}
  CDriveControl(CDriveAxis *ADriveAxis);

  void move_forward();
  void move_backward();
  void turn_left();
  void turn_right();

  CDriveAxis *DriveAxis;
  
};

#endif // CDRIVECONTROL_H
