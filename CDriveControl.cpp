#include "CDriveControl.h"

CDriveControl::CDriveControl(CDriveAxis *ADriveAxis, byte left_rot_pin, byte right_rot_pin)
{
  DriveAxis = ADriveAxis;
  left_rot_pin = left_rot_pin;
  right_rot_pin = right_rot_pin;
  Serial.print("set forward speed to ");
  Serial.println(SPEED);
  Serial.print("set turn speed to ");
  Serial.println(TURN_SPEED);
  Serial.print("front wheel turn angle is ");
  Serial.println(TURN_ANGLE);
  servol.attach(left_rot_pin);
  servol.write(FORWARD_ANGLE);
  servor.attach(right_rot_pin);
  servor.write(FORWARD_ANGLE);
  DriveAxis->stop();

  BlackLinePassStatus = EBLPS_BOTH_EXPECTING;
  passed_black_lines = 0;
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

void CDriveControl::move_counting_lines(int dist_in_lines)
{
  maneuver_time = millis();

  BlackLinePassStatus = EBLPS_BOTH_EXPECTING;
  Maneuver = EMS_DIRECTING_TURN;
  
  black_lines_count_to_pass = abs(dist_in_lines) - 1;
  passed_black_lines = 0;
  
  turn_time = 800 + (MAX_TURN_TIME - 800) / black_lines_count_to_pass;
  if (turn_time > MAX_TURN_TIME) turn_time = MAX_TURN_TIME;
  if (black_lines_count_to_pass == 0)
  {
    turn_time = MIN_TURN_TIME;
    // начальное ускорение
    // TODO учесть и для больших поворотов
    if (one_time_acceleration == false)
    {
      turn_time = MIN_TURN_TIME + 200;
      one_time_acceleration = true;
    }
  }
    
  Serial.print("\thead vector in lines is ");
  Serial.println(dist_in_lines);
  Serial.print("\tblack_lines_count_to_pass is ");
  Serial.println(black_lines_count_to_pass);
  Serial.print("\tpassed_black_lines is ");
  Serial.println(passed_black_lines);
  Serial.print("\tturn_time is ");
  Serial.println(turn_time);
  Serial.print("\tBlackLinePassStatus is ");
  Serial.println(BlackLinePassStatus);
  Serial.print("\tManeuver is ");
  Serial.println(Maneuver);
  

  // инициируем первый поворот
  if (dist_in_lines < 0)
  {
    
    left_turn = true;
    Serial.println("first turn to LEFT");
    turn_left(TURN_ANGLE);
  }
  else 
  {
    left_turn = false;
    Serial.println("first turn to RIGHT");
    turn_right(TURN_ANGLE);
  }    
}

byte CDriveControl::loop(int leftEn, int rightEn)
{
  switch (Maneuver)
  {
    // совершаем поворот (начальный)
    case EMS_DIRECTING_TURN:
    {
      if (millis() - maneuver_time > turn_time)
      {
        move_forward();
        Maneuver = EMS_COUNTING_BLACK_LINES;
        Serial.println("DONE First TURN!");
        if (black_lines_count_to_pass == 0)
        {
          start_second_turn();
        }        
        return DIRECTING_TURN_OVER;
      }
      search_lines(leftEn, rightEn);
      return DIRECTING_TURN;
    }

    // едем наискосок к черным полоским, считая их
    case EMS_COUNTING_BLACK_LINES:
    {
      // левый/правый датчики приводим к ведущему/ведомому
      search_lines(leftEn, rightEn);
      return COUNTING_BLACK_LINES;
    }

    // совершаем выравнивающий (второй) поворот
    case EMS_FINAL_TURN:
    {
//      if ((black_lines_count_to_pass == 0) && (leftEn == HIGH || rightEn == HIGH))
//      {
//        move_forward();
//        Maneuver = EMS_ALONG_BLACK_LINE;
//        Serial.println("INTERRUPT FINAL turn");
//        
//        return FINAL_TURN_OVER;
//      }
      if ( millis() - maneuver_time > turn_time + 100)
      {
        move_forward();
        Maneuver = EMS_ALONG_BLACK_LINE;
        Serial.println("DONE FINAL turn");
        return FINAL_TURN_OVER;
      }
      return FINAL_TURN;
    }
  }
}

void CDriveControl::start_second_turn()
{
  Maneuver = EMS_FINAL_TURN;
  maneuver_time = millis();
  
  if (left_turn)
  {
    Serial.println("second turn to RIGHT");
    turn_right(TURN_ANGLE / SECOND_TURN_ANGLE_DECREASE_FACTOR);
  }
  else
  {
    Serial.println("second turn to LEFT");
    turn_left(TURN_ANGLE / SECOND_TURN_ANGLE_DECREASE_FACTOR);
  }
    
}

void CDriveControl::search_lines(int leftEn, int rightEn)
{
  if (millis() - maneuver_time < TURN_TIME_CROSS_LINE) return;
  
  if (left_turn) { byOrder[0] = leftEn; byOrder[1] = rightEn; }
  else { byOrder[1] = leftEn; byOrder[0] = rightEn; }
  
  switch (BlackLinePassStatus)
  {
    case EBLPS_BOTH_EXPECTING:{
      if (byOrder[0] == 1)
        BlackLinePassStatus = EBLPS_FIRST_SENSOR_DETECTION;
      return;
    }
    case EBLPS_FIRST_SENSOR_DETECTION:{
      if (byOrder[0] == 0 && byOrder[1] == 0)
        BlackLinePassStatus = EBLPS_FIRST_SENSOR_PASSED_SECOND_EXPECTING;
      else if (byOrder[0] == 1 && byOrder[1] == 1)
        BlackLinePassStatus = EBLPS_BOTH_SENSORS_DETECTION;
      return;
    }
    case EBLPS_FIRST_SENSOR_PASSED_SECOND_EXPECTING:{
      if (byOrder[0] == 0 && byOrder[1] == 1)
        BlackLinePassStatus = EBLPS_FIRST_SENSOR_PASSED_SECOND_DETECTION;
      return;
    }
    case EBLPS_BOTH_SENSORS_DETECTION:{
      if (byOrder[0] == 0)
        BlackLinePassStatus = EBLPS_FIRST_SENSOR_PASSED_SECOND_DETECTION;
      return;
    }
    case EBLPS_FIRST_SENSOR_PASSED_SECOND_DETECTION:{
      if (byOrder[0] == 0 && byOrder[1] == 0)
      {
        BlackLinePassStatus = EBLPS_BOTH_EXPECTING;
        passed_black_lines++;
        Serial.print("Got one more black line to ");
        Serial.println(passed_black_lines);
        if (passed_black_lines == black_lines_count_to_pass)
        {  // прошли все линии -> выруливаем на следующую черную полосу
          Serial.println("DONE BLACK LINES counting");
          start_second_turn();
        }
      }
      return;
    }
  }
}
