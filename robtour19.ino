#include <IRremote.h>

#include "CDriveAxis.h"
#include "CDriveControl.h"

// датчики линии
const byte left_line_follower_pin = A1;
const byte right_line_follower_pin = A0;

// сервопривод передней оси поворота робота
const byte servo_rotate_chassis = 2;

// мотор ведущей оси
const byte motor_drive_pinA = 4;
const byte motor_drive_pinB = 5;
const byte motor_drive_pinE = 3;
const byte STBY = 12; // standby

// приёмник ИК-сигнала
const byte ir_rc_pin = 11;
IRrecv irrecv(ir_rc_pin);
decode_results results;

CDriveAxis *DriveAxis;
CDriveControl DriveControl;
MovementDirection last_move_cmd = MMD_NONE;

long last_time = 0;

void setup()
{

  pinMode(2, OUTPUT); // servo rotation chassis
  pinMode(3, OUTPUT);// enable drive axis motor
  pinMode(4, OUTPUT);// pinA drive axis motor
  pinMode(5, OUTPUT);// pinB drive axis motor
  pinMode(STBY, OUTPUT); // 12 standby drive axis motor
  
  pinMode(14, INPUT);// pinA0 // in3 for motor2
  pinMode(15, INPUT);// pinA1 // switch(right) for чего-нибудь/ если его включить, то на А1 будет подаваться Vin (7.2В)


  Serial.begin(115200);
  
  DriveAxis = new CDriveAxis(motor_drive_pinA, motor_drive_pinE, motor_drive_pinB, STBY); // TODO
  DriveControl = CDriveControl(DriveAxis, servo_rotate_chassis); // TODO

  irrecv.enableIRIn();

  last_time = millis();
}

void loop()
{

  if (irrecv.decode(&results)) {
        Serial.print("0x");
        Serial.println(results.value, HEX);
        delay(50);
        irrecv.resume();// Receive the next value
    }


    
//  DriveControl.move_forward();
  long dt = millis() - last_time;
  if ( dt < 5000)
    DriveControl.turn_right();
  else if (dt < 10000)
    DriveControl.move_forward();
  else if (dt < 15000)
    DriveControl.turn_left();
  move_by_line();
}

// движение по черной полосе
void move_by_line()
{
  int leftLF = digitalRead(left_line_follower_pin);
  int rightLF = digitalRead(right_line_follower_pin);

//  Serial.print(leftLF);
//  Serial.print("\t");
//  Serial.println(rightLF);

  if (leftLF == LOW && rightLF == LOW)
  {
    DriveControl.move_forward();
    last_move_cmd = MMD_FORWARD;
//    Serial.println("to FORWARD");
//    confused_threshold-=10;
  }
  else if (leftLF == HIGH && rightLF == HIGH)
  {
//    if (confused_threshold > 1000)
//    {
//      DriveControl.move_forward();
//      Serial.println("FORCE FORCE FORCE FORWARD");
//      confused_threshold;
//    }
    if (last_move_cmd == MMD_TURN_RIGHT)
    {
      DriveControl.turn_right(); 
      last_move_cmd = MMD_TURN_RIGHT;
//      Serial.println("repeat RIGHT");
//      confused_threshold++;
    }
    else if (last_move_cmd == MMD_TURN_LEFT)
    {
      DriveControl.turn_left(); 
      last_move_cmd = MMD_TURN_LEFT;
//      Serial.println("repeat LEFT");
//      confused_threshold++;
    }
    else
    { // произошло нечто неординарное
//      Serial.println("ЧЕРНАЯ ПОЛОСА ПОПЕРЕК");
      }
  }
  else if (leftLF == LOW && rightLF == HIGH)
  {
    DriveControl.turn_right();
    last_move_cmd = MMD_TURN_RIGHT;
//    Serial.println("to RIGHT");
  }
  else if (leftLF == HIGH && rightLF == LOW)
  {
    DriveControl.turn_left();
    last_move_cmd = MMD_TURN_LEFT;
//    Serial.println("to LEFT");
  }
}
