#ifndef CDRIVEAXIS_H
#define CDRIVEAXIS_H

#include <Arduino.h>


// Ось с приводом (без энкодера). Отвечет за движение робота вперед/назад
class CDriveAxis
{
public:
  CDriveAxis() {}
  CDriveAxis(byte pin_motor_A, byte pin_speed, byte pin_motor_B, byte pin_standby);

  // установить направление и скорость вращения ведущей оси
  void forward(); // движение вперед
  void backward(); // движение назад
  void stop(); // остановка движения

  // задать текущую скорость вращения jcb
  void setSpeed(byte speed);
  byte getSpeed();

private:
  // скорость вращения оси в диапазоне [0 .. 255]
  byte _speed;
  
  byte _pin_motor_A;
  byte _pin_speed;
  byte _pin_motor_B;
  byte _pin_standby;
  
  // Направление вращения колеса
  enum EWheelRotationDirection
  {
    EWRD_STOP,    // не вращается
    EWRD_FORWARD, // вперед
    EWRD_BACKWARD // назад
  } _rotation_direction;
  
};

#endif // CDRIVEAXIS_H
