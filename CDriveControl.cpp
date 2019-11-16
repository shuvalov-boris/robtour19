#include "CDriveControl.h"

CDriveControl::CDriveControl(CDriveAxis *ADriveAxis)
{
  DriveAxis = ADriveAxis;
}

void CDriveControl::move_forward()
{
  DriveAxis->setSpeed(100);
  DriveAxis->forward();
}

void CDriveControl::move_backward()
{
  
}

void CDriveControl::turn_left()
{
  
}

void CDriveControl::turn_right()
{
  
}
