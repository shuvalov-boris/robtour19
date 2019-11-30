#include "CTowerControl.h"

CTowerControl::CTowerControl(const CGearMotor *AGripperLift, const byte gripper_pin, const byte tower_tilt_pin, const byte rope_tension_pin)
{
  GripperLift = AGripperLift;
  
  servo_gripper.attach(gripper_pin);
  servo_gripper.write(0);
  
  servo_tower_tilt.attach(tower_tilt_pin);
  servo_tower_tilt.write(0);

  servo_rope_tension.attach(rope_tension_pin);
//  servo_gripper.write(0);
  
  coins_manipulation_phase = ECMP_COLLECTING;
  gripper_time = tower_time = millis();
}

void CTowerControl::Loop()
{
  switch (coins_manipulation_phase)
  {
    case ECMP_LIFTING:
      if (millis() - gripper_time > GRIPPER_TILT_DELAY && gripper_angle < GRIPPER_TARGET_TILT) // TODO add delay
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
      if (millis() - tower_time > TOWER_TILT_DELAY && tower_angle < TOWER_TARGET_TILT) // add delay
      {
        servo_tower_tilt.write(++tower_angle);
        tower_time = millis();
//        delay(100);
  //      last_btn_state = LOW;
//        Serial.println(tower_angle);
//        Serial.read();
      }
      else
        coins_manipulation_phase = ECMP_COMPLETED;
      break;
  }
}

void CTowerControl::Start()
{
  GripCoins();
}

void CTowerControl::ProcessCommand(int incomingByte)
{
  if (tower_angle < 20 && incomingByte == 50)
  {
    servo_tower_tilt.write(++tower_angle);
    delay(50);
    Serial.println(tower_angle);
  }

  if (tower_angle < 40 && incomingByte == 51)
  {
    servo_tower_tilt.write(++tower_angle);
    delay(100);
    Serial.println(tower_angle);
  }

  if (tower_angle < 60 && incomingByte == 52)
  {
    servo_tower_tilt.write(++tower_angle);
    delay(100);
    Serial.println(tower_angle);
  }

  
  if (tower_angle < 80 && incomingByte == 53)
  {
    servo_tower_tilt.write(++tower_angle);
    delay(100);
    Serial.println(tower_angle);
  }
  
  if (tower_angle < 90 && incomingByte == 54)
  {
    servo_tower_tilt.write(++tower_angle);
    delay(100);
    Serial.println(tower_angle);
  }
  
  
  if (incomingByte == 119)
  {
    servo_tower_tilt.write(++tower_angle);
    delay(100);
    Serial.println(tower_angle);
    incomingByte = 0;
  }

  if (tower_angle > 0 && incomingByte == 55)
  {
    servo_tower_tilt.write(--tower_angle);
    delay(100);
    Serial.println(tower_angle);
  }

  if (gripper_angle < 100 && incomingByte == 117) // lift up [u]
  {
    servo_gripper.write(++gripper_angle);
    delay(10);
    Serial.println(gripper_angle);
  }

  if (gripper_angle > 0 && incomingByte == 100) // lift down [d]
  {
    servo_gripper.write(--gripper_angle);
    delay(10);
    Serial.println(gripper_angle);
  }
}

void CTowerControl::GripCoins()
{
  coins_manipulation_phase = ECMP_GRIPPING;
//      if (gripper_angle < 100 && incomingByte == 117) // lift up [u]
    
  servo_gripper.write(0);

  coins_manipulation_phase = ECMP_LIFTING;
  
}

void CTowerControl::LiftGripper()
{
  coins_manipulation_phase = ECMP_LIFTING;

  GripperLift->setSpeed(255);
  // подъём лапы с фишками
  GripperLift->backward();
}

void CTowerControl::TiltTower()
{
  coins_manipulation_phase = ECMP_TOWER_TILT;
}
