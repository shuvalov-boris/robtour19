#include "CDriveControl.h"

CDriveControl::CDriveControl(CDriveAxis *ADriveAxis, byte left_rot_pin, byte right_rot_pin)
{
  DriveAxis = ADriveAxis;
  left_rot_pin = left_rot_pin;
  right_rot_pin = right_rot_pin;
  servol.attach(left_rot_pin);
  servol.write(FORWARD_ANGLE);
  servor.attach(right_rot_pin);
  servor.write(FORWARD_ANGLE);
}

void CDriveControl::move_forward()
{
  servol.write(FORWARD_ANGLE);
  servor.write(FORWARD_ANGLE);
  DriveAxis->setSpeed(80);
  DriveAxis->forward();
}

void CDriveControl::move_backward()
{
  servol.write(FORWARD_ANGLE);
  servor.write(FORWARD_ANGLE);
  DriveAxis->setSpeed(80);
  DriveAxis->backward();
}

void CDriveControl::turn_left(byte aangle)
{
  servol.write(FORWARD_ANGLE - aangle);
  servor.write(FORWARD_ANGLE - aangle);
  DriveAxis->setSpeed(80);
  DriveAxis->forward();
}

void CDriveControl::turn_right(byte aangle)
{
  servol.write(FORWARD_ANGLE + aangle);
  servor.write(FORWARD_ANGLE + aangle);
  DriveAxis->setSpeed(80);
  DriveAxis->forward();
}

void CDriveControl::move_counting_lines(byte dist_in_lines)
{
  maneuver_time = millis();
  black_lines_count_to_pass = abs(dist_in_lines) - 1;
  
  if (dist_in_lines < 0)
  {
    Maneuver = EMS_DIRECTING_TURN;
    left_turn = true;
    turn_left();
  }
  else 
  {
    Maneuver = EMS_DIRECTING_TURN;
    left_turn = false;
    turn_right();
  }    
}

byte CDriveControl::loop(int leftEn, int rightEn)
{
  switch (Maneuver)
  {
    // совершаем поворот (начальный)
    case EMS_DIRECTING_TURN:
      if (millis() - maneuver_time > TURN_TIME)
      {
        move_forward();
        Maneuver = EMS_COUNTING_BLACK_LINES;
        passed_black_lines = 0; // точно ли?
        BlackLinePassStatus = EBLPS_BOTH_EXPECTING; // TODO если мы еще не доехали до первой черной полосы НЕ ФАКТ!!!
        // TODO мониторить ли тут черные полосы?
        Serial.print("First TURN passed!");
        return DIRECTING_TURN_OVER;
      }
      break;

    // едем наискосок к черным полоским, считая их
    case EMS_COUNTING_BLACK_LINES:
      // левый/правый датчики приводим к ведущему/ведомому
      if (left_turn) { byOrder[0] = leftEn; byOrder[1] = rightEn; }
      else { byOrder[1] = leftEn; byOrder[0] = rightEn; }
      switch (BlackLinePassStatus)
      {
        case EBLPS_BOTH_EXPECTING:
          if (byOrder[0] == 1)
            BlackLinePassStatus = EBLPS_FIRST_SENSOR_DETECTION;
          break;
        case EBLPS_FIRST_SENSOR_DETECTION:
          if (byOrder[0] == 0 && byOrder[1] == 0)
            BlackLinePassStatus = EBLPS_FIRST_SENSOR_PASSED_SECOND_EXPECTING;
          else if (byOrder[0] == 1 && byOrder[1] == 1)
            BlackLinePassStatus = EBLPS_BOTH_SENSORS_DETECTION;
          break;
        case EBLPS_FIRST_SENSOR_PASSED_SECOND_EXPECTING:
          if (byOrder[0] == 0 && byOrder[1] == 1)
            BlackLinePassStatus = EBLPS_FIRST_SENSOR_PASSED_SECOND_DETECTION;
          break;
        case EBLPS_BOTH_SENSORS_DETECTION:
          if (byOrder[0] == 0)
            BlackLinePassStatus = EBLPS_FIRST_SENSOR_PASSED_SECOND_DETECTION;
          break;
        case EBLPS_FIRST_SENSOR_PASSED_SECOND_DETECTION:
          if (byOrder[0] == 0 && byOrder[1] == 0)
          {
            BlackLinePassStatus = EBLPS_BOTH_EXPECTING;
            passed_black_lines++;
            Serial.print("Got one more black line to ");
            Serial.println(passed_black_lines);
            if (passed_black_lines == black_lines_count_to_pass)
            {  // прошли все линии -> выруливаем на следующую черную полосу
              Maneuver = EMS_FINAL_TURN;
              maneuver_time = millis();
              if (left_turn)
                turn_right();
              else
                turn_left();
            }
          }
      }
      break;

    // совершаем выравнивающий (второй) поворот
    case EMS_FINAL_TURN:
      if (millis() - maneuver_time > TURN_TIME)
      {
        move_forward();
        Maneuver = EMS_ALONG_BLACK_LINE;
        
        return FINAL_TURN_OVER;
      }
      break;
  }
}
