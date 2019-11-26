#include  "CGearMotor.h"

CGearMotor::CGearMotor(byte pin_motor_A, byte pin_speed, byte pin_motor_B, byte pin_standby) 
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
void CGearMotor::forward() {
  if (_rotation_direction != EWRD_FORWARD) {
    _rotation_direction = EWRD_FORWARD;
    digitalWrite(_pin_motor_A, 1);
//    _speed = 255;
    analogWrite(_pin_speed, _speed);
    digitalWrite(_pin_motor_B, 0);
  }
}

// движение колеса назад
void CGearMotor::backward() {
  if (_rotation_direction != EWRD_BACKWARD) {
    _rotation_direction = EWRD_BACKWARD;   
    _speed = 255; 
    digitalWrite(_pin_motor_A, 0);
    analogWrite(_pin_speed, _speed);
    digitalWrite(_pin_motor_B, 1);
  }
}

// остановить движение колеса
void CGearMotor::stop()
{
  if (_rotation_direction != EWRD_STOP) {
    _rotation_direction = EWRD_STOP;
    _speed = 0;
    analogWrite(_pin_speed, _speed);
  }
}

void CGearMotor::setSpeed(byte aspeed){
  analogWrite(_pin_speed, aspeed);
  _speed = aspeed;  
}

byte CGearMotor::getSpeed(){
  return _speed;
}
