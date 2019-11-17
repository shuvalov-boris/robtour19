#include "CDriveAxis.h"

CDriveAxis::CDriveAxis(byte pin_motor_A, byte pin_speed, byte pin_motor_B, byte pin_standby) 
{
  _pin_motor_A = pin_motor_A;
  _pin_speed = pin_speed;
  _pin_motor_B = pin_motor_B;
  _pin_standby = pin_standby;
  pinMode(_pin_motor_A, OUTPUT);
  pinMode(_pin_speed, OUTPUT);
  pinMode(_pin_motor_B, OUTPUT);
  digitalWrite(_pin_standby, HIGH);
  stop();
}

// движение колеса вперед
void CDriveAxis::forward() {
  if (_rotation_direction != EWRD_FORWARD) {
    _rotation_direction = EWRD_FORWARD;
    digitalWrite(_pin_motor_A, 1);
    _speed = 80;
    analogWrite(_pin_speed, _speed);
    digitalWrite(_pin_motor_B, 0);
  }
}

// движение колеса назад
void CDriveAxis::backward() {
  if (_rotation_direction != EWRD_BACKWARD) {
    _rotation_direction = EWRD_BACKWARD;   
    _speed = 80; 
    digitalWrite(_pin_motor_A, 0);
    analogWrite(_pin_speed, _speed);
    digitalWrite(_pin_motor_B, 1);
  }
}

// остановить движение колеса
void CDriveAxis::stop()
{
  if (_rotation_direction != EWRD_STOP) {
    _rotation_direction = EWRD_STOP;
    _speed = 0;
    analogWrite(_pin_speed, _speed);
  }
}

void CDriveAxis::setSpeed(byte aspeed){
  analogWrite(_pin_speed, aspeed);
  _speed = aspeed;  
}

byte CDriveAxis::getSpeed(){
  return _speed;
}
