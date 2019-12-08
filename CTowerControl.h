#pragma once

#include "Arduino.h"
#include "Servo.h"

#include "CGearMotor.h"


#define GRIPPER_START_TILT 20
#define GRIPPER_TARGET_TILT 120
#define GRIPPER_TILT_DELAY 10
#define GRIPPER_LIFT_TIME 3000

// угол целевого наклона башни для закрепления фишек на мишени
#define TOWER_TARGET_TILT 60
// скорость наклона
#define TOWER_TILT_DELAY 50


class CTowerControl
{
public:
  CTowerControl() {}
  CTowerControl(const CGearMotor *AGripperLift, const byte gripper_pin, const byte tower_tilt_pin, const byte rope_tension_pin);

  int Loop();

  void Start();

  // Для отладки
  void ProcessCommand(int cmd);

private:
  // стадия работы с фишками
  enum ECoinsManipulationPhase
  {
    ECMP_COLLECTING, // сбор фишек
    ECMP_GRIPPING,   // захват фишек
    ECMP_LIFTING,    // подъём вверх лапы с фишками
    ECMP_GRIPPER_TILTING, // поворот вверх лапы, чтобы фишки оказались параллельно мишени
    ECMP_TOWER_TILT, // наклон башни вперёд, чтобы фишки оказались параллельно мишени
    ECMP_COMPLETED   // задание выполнено
  } coins_manipulation_phase;

  long gripper_time;
  long gripper_lift_time;
  long tower_time;

  int tower_angle = 0;
  int gripper_angle = 0;

  CGearMotor *GripperLift;

  Servo servo_gripper; //TODO init
  Servo servo_rope_tension; //TODO init
  Servo servo_tower_tilt; //TODO

  void GripCoins();

  void LiftGripper();

  void TiltGripper();

  void TiltTower();

};
