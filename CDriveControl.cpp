#include "CDriveControl.h"

CDriveControl::CDriveControl(CDriveAxis *ADriveAxis, byte servo_pin)
{
  DriveAxis = ADriveAxis;
  _servo_pin = servo_pin;
  servo.attach(_servo_pin);
}

void CDriveControl::move_forward()
{
  servo.write(90);
  DriveAxis->setSpeed(80);
  DriveAxis->forward();
}

void CDriveControl::move_backward()
{
  
}

void CDriveControl::turn_left()
{
  servo.write(60);
  DriveAxis->setSpeed(80);
  DriveAxis->forward();
}

void CDriveControl::turn_right()
{
  servo.write(120);
  DriveAxis->setSpeed(80);
  DriveAxis->forward();
}
