#include "CTowerControl.h"

CTowerControl::CTowerControl(const CGearMotor *AGripperLift, const byte gripper_pin, const byte tower_tilt_pin, const byte rope_tension_pin)
{
  GripperLift = AGripperLift;
  
  servo_gripper.attach(gripper_pin);
  servo_gripper.write(GRIPPER_START_TILT);
  
  servo_tower_tilt.attach(tower_tilt_pin);
  servo_tower_tilt.write(0);

  servo_rope_tension.attach(rope_tension_pin);
//  servo_gripper.write(0);
  
  coins_manipulation_phase = ECMP_COLLECTING;
  gripper_time = tower_time = millis();
  Serial.println("CTowerControl constructor");

  coins_manipulation_phase = ECMP_COLLECTING;
}

int CTowerControl::Loop()
{
  switch (coins_manipulation_phase)
  {
    // поднимаем платформу
    case ECMP_LIFTING:
      if (millis() - gripper_lift_time > GRIPPER_LIFT_TIME)
      {
        GripperLift->stop();
        coins_manipulation_phase = ECMP_GRIPPER_TILTING;
      }
      break;
      
    case ECMP_GRIPPER_TILTING:
      if (millis() - gripper_time < GRIPPER_TILT_DELAY)
        break;
      if (gripper_angle < GRIPPER_TARGET_TILT)
      {
        servo_gripper.write(++gripper_angle);
        gripper_time = millis();
        
  //      last_btn_state = LOW;
//        Serial.println(gripper_angle);
//        Serial.read();
      }
      else
        TiltTower();
      break;

    case ECMP_TOWER_TILT:   
      if (millis() - tower_time < TOWER_TILT_DELAY)
        break;
      if (tower_angle < TOWER_TARGET_TILT)
      {
        servo_tower_tilt.write(++tower_angle);
        tower_time = millis();
//        delay(100);
  //      last_btn_state = LOW;
//        Serial.println(tower_angle);
//        Serial.read();
      }
      else {
        coins_manipulation_phase = ECMP_COMPLETED;
        return 1;
      }
      break;
  }
  return 0;
}

void CTowerControl::Start()
{
  Serial.println("CTowerControl::Start");
  GripCoins();  
}

void CTowerControl::ProcessCommand(int cmd)
{
//  Serial.print("TowerControl: got command ");
//  Serial.println(cmd);
  
  if (tower_angle < 20 && cmd == 50) // 2 - наклон башни до 20 градусов
  {
    servo_tower_tilt.write(++tower_angle);
    delay(50);
    Serial.print("tower tilt angle is ");
    Serial.println(tower_angle);
  }

  if (tower_angle < 40 && cmd == 51) // 3 - наклон башни до 40 градусов
  {
    servo_tower_tilt.write(++tower_angle);
    delay(100);
    Serial.print("tower tilt angle is ");
    Serial.println(tower_angle);
  }

  if (tower_angle < 60 && cmd == 52)  // 4 - наклон башни до 60 градусов
  {
    servo_tower_tilt.write(++tower_angle);
    delay(100);
    Serial.print("tower tilt angle is ");
    Serial.println(tower_angle);
  }

  
  if (tower_angle < 80 && cmd == 53)
  {
    servo_tower_tilt.write(++tower_angle);
    delay(100);
    Serial.print("tower tilt angle is ");
    Serial.println(tower_angle);
  }
  
  if (tower_angle < 90 && cmd == 54)
  {
    servo_tower_tilt.write(++tower_angle);
    delay(100);
    Serial.print("tower tilt angle is ");
    Serial.println(tower_angle);
  }
  
  
  if (cmd == 119) // w
  {
    servo_tower_tilt.write(++tower_angle);
    delay(50);
    Serial.print("tower tilt angle is ");
    Serial.println(tower_angle);
    cmd = 0;
  }

  if (tower_angle > 0 && cmd == 55) // [7] - подъём башни в нулевое положение
  {
    servo_tower_tilt.write(--tower_angle);
    delay(50);
    Serial.print("tower tilt angle is ");
    Serial.println(tower_angle);
  }

  if (gripper_angle < 100 && cmd == 117) // lift up [u]
  {
    servo_gripper.write(++gripper_angle);
    delay(10);
    Serial.print("gripper angle is ");
    Serial.println(gripper_angle);
  }

  if (gripper_angle > 0 && cmd == 100) // lift down [d]
  {
    servo_gripper.write(--gripper_angle);
    delay(10);
    Serial.print("gripper angle is ");
    Serial.println(gripper_angle);
  }

  if (cmd == 97) // [a] платформу вверх
  {
    GripperLift->setSpeed(255);
  // подъём лапы с фишками
    GripperLift->forward();
    Serial.println('gripper up');
  }

  if (cmd == 122) // [z] платформу вниз
  {
    GripperLift->setSpeed(255);
  // подъём лапы с фишками
    GripperLift->backward();
    Serial.println('gripper down');
  }
}

void CTowerControl::GripCoins()
{
  Serial.println("CTowerControl::GripCoins");
  coins_manipulation_phase = ECMP_GRIPPING;

  // опускаем лапу для захвата фишек
  GripperLift->setSpeed(100);  
  GripperLift->backward();
  delay(120);
  GripperLift->stop();
  Serial.println('gripper is down');

  // наклоняем лапу, чтобы точно взять фишки
  servo_gripper.write(0);
  delay(500);
  servo_gripper.write(15);
  delay(500);
  servo_gripper.write(0);
  delay(500);
  servo_gripper.write(15);
  delay(500);

  GripperLift->backward();
  delay(70);
  GripperLift->stop();

  servo_gripper.write(0);
  delay(500);
  servo_gripper.write(15);
  delay(500);

  LiftGripper();  
}

void CTowerControl::LiftGripper()
{
  coins_manipulation_phase = ECMP_LIFTING;
  gripper_lift_time = millis();

  GripperLift->setSpeed(255);
  // подъём лапы с фишками
  GripperLift->forward();
}

void CTowerControl::TiltTower()
{
  coins_manipulation_phase = ECMP_TOWER_TILT;
}
