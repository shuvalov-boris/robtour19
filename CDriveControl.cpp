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
  DriveAxis->stop();
}

void CDriveControl::move_forward()
{
  servol.write(FORWARD_ANGLE);
  servor.write(FORWARD_ANGLE);
  DriveAxis->setSpeed(SPEED);
  DriveAxis->forward();
}

void CDriveControl::move_backward()
{
  servol.write(FORWARD_ANGLE);
  servor.write(FORWARD_ANGLE);
  DriveAxis->setSpeed(SPEED);
  DriveAxis->backward();
}

void CDriveControl::turn_left(byte aangle)
{
  servol.write(FORWARD_ANGLE - aangle);
  servor.write(FORWARD_ANGLE - aangle);
  DriveAxis->setSpeed(TURN_SPEED);
  DriveAxis->forward();
}

void CDriveControl::turn_right(byte aangle)
{
  servol.write(FORWARD_ANGLE + aangle);
  servor.write(FORWARD_ANGLE + aangle);
  DriveAxis->setSpeed(TURN_SPEED);
  DriveAxis->forward();
}

void CDriveControl::move_counting_lines(byte dist_in_lines)
{
  maneuver_time = millis();
  black_lines_count_to_pass = abs(dist_in_lines) - 1;
  Serial.print("black_lines_count_to_pass is ");
  Serial.println(black_lines_count_to_pass);

  turn_time = 780 + black_lines_count_to_pass * 110;
  if (turn_time > 1250) turn_time = 1250;
  if (black_lines_count_to_pass == 0)
    turn_time = 400;

  passed_black_lines = 0; // точно ли?
  BlackLinePassStatus = EBLPS_BOTH_EXPECTING;
        
  if (dist_in_lines < 0)
  {
    Maneuver = EMS_DIRECTING_TURN;
    left_turn = true;
    Serial.println("CDriveControl LEFT");
    turn_left(TURN_ANGLE);
  }
  else 
  {
    Maneuver = EMS_DIRECTING_TURN;
    left_turn = false;
    Serial.println("CDriveControl RIGHT");
    turn_right(TURN_ANGLE);
  }    
}

byte CDriveControl::loop(int leftEn, int rightEn)
{
  switch (Maneuver)
  {
    // совершаем поворот (начальный)
    case EMS_DIRECTING_TURN:
      if (millis() - maneuver_time > turn_time)
      {
        move_forward();
        Maneuver = EMS_COUNTING_BLACK_LINES;
        if (black_lines_count_to_pass == 0)
          Maneuver = EMS_FINAL_TURN;
         // TODO если мы еще не доехали до первой черной полосы НЕ ФАКТ!!!
        // TODO мониторить ли тут черные полосы?
        Serial.println("First TURN passed!");
        return DIRECTING_TURN_OVER;
      }
      search_lines(leftEn, rightEn);
      break;

    // едем наискосок к черным полоским, считая их
    case EMS_COUNTING_BLACK_LINES:
      // левый/правый датчики приводим к ведущему/ведомому
      search_lines(leftEn, rightEn);
      break;

    // совершаем выравнивающий (второй) поворот
    case EMS_FINAL_TURN:
      if ((black_lines_count_to_pass == 0) && (leftEn == HIGH || rightEn == HIGH))
      {
        move_forward();
        Maneuver = EMS_ALONG_BLACK_LINE;
        Serial.println("INTERRUPT FIMAL turn");
        
        return FINAL_TURN_OVER;
      }
      if ( millis() - maneuver_time > turn_time)
      {
        move_forward();
        Maneuver = EMS_ALONG_BLACK_LINE;
        Serial.println("FINAL turn completed");
        return FINAL_TURN_OVER;
      }
      break;
  }
}

void CDriveControl::search_lines(int leftEn, int rightEn)
{
  if (millis() - maneuver_time < 300) return;
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

//            if (passed_black_lines + 1 >= black_lines_count_to_pass)
//              turn_add = TURN_ANGLE / 2;
            if (passed_black_lines == black_lines_count_to_pass)
            {  // прошли все линии -> выруливаем на следующую черную полосу
              Serial.println("ALL BLACK LINES");
              Maneuver = EMS_FINAL_TURN;
              maneuver_time = millis();
            
              if (left_turn)
                turn_right(TURN_ANGLE / 3);
              else
                turn_left(TURN_ANGLE / 3);
            }
          }
      }
}
