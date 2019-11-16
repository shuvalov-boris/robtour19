#ifndef CDRIVECONTROL_H
#define CDRIVECONTROL_H

#include <Servo.h>
#include "Arduino.h"
#include "CDriveAxis.h"

#define TURN_TIME 500 // [ms]

// return codes of loop()
#define DIRECTING_TURN_OVER 101 // первый поворот окончен
#define FINAL_TURN_OVER 103 // второй поворот окончен


const byte TURN_ANGLE = 25;
const byte FORWARD_ANGLE = 82; // угол сервопривода для езды прямо

// направление движения робота
enum EMovementDirection {
  EMD_NONE,  
  EMD_FORWARD, 
  EMD_TURN_LEFT, 
  EMD_TURN_RIGHT, 
  EMD_ROTATE_LEFT, 
  EMD_ROTATE_RIGHT, 
  EMD_BACKWARD,
  EMD_STOP
};

// Маневр - совокупность движений по достижению следующей синей полосы
enum EManeuverStatus
{
  EMS_DIRECTING_TURN,
  EMS_COUNTING_BLACK_LINES,
  EMS_FINAL_TURN,
  EMS_ALONG_BLACK_LINE // до синей полосы (или финиша)
};

enum EBlackLinePassStatus
{
  EBLPS_BOTH_EXPECTING,                         //    0 0
  EBLPS_FIRST_SENSOR_DETECTION,                 //    0 1
  EBLPS_FIRST_SENSOR_PASSED_SECOND_EXPECTING,   //    0 2
  EBLPS_BOTH_SENSORS_DETECTION,                 //    1 1
  EBLPS_FIRST_SENSOR_PASSED_SECOND_DETECTION,   //    1 2       
  EBLPS_BOTH_SENSORS_PASSED                     //    2 2
};

class CDriveControl
{
public: 
  CDriveControl() {}
  CDriveControl(CDriveAxis *ADriveAxis, byte left_rot_pin, byte right_rot_pin);

  // low logic
  void move_forward();
  void move_backward();
  void turn_left();
  void turn_right();

  // high logic
  void move_counting_lines(byte dy);
  
  byte loop(int leftEn, int rightEn);

private:
  CDriveAxis *DriveAxis;
  byte left_rot_pin;
  byte right_rot_pin;
  Servo servol;
  Servo servor;

  // для небольших поворотов на 1-7 линий
  unsigned long maneuver_time;
  byte black_lines_count_to_pass;
  bool left_turn;
  EManeuverStatus Maneuver;
  EBlackLinePassStatus BlackLinePassStatus;
  byte passed_black_lines; // количество пересеченных черных линий (стартовая (1-ая) не считается)
  byte byOrder[2];
  
};

#endif // CDRIVECONTROL_H
