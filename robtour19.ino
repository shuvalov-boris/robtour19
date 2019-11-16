#include <IRremote.h>

#include "CColorTracker.h"
#include "CDriveAxis.h"
#include "CDriveControl.h"

// датчики линии
const byte left_line_follower_pin = A1;
const byte right_line_follower_pin = A0;

// датчики цвета
const byte S0 = 9;
const byte S1 = 8;
const byte S2 = 7;
const byte S3 = 6;
const byte color_out = 10;


// сервопривод передней оси поворота робота
const byte servo_rotate_chassis = 2;

// мотор ведущей оси
const byte motor_drive_pinA = 4;
const byte motor_drive_pinB = 5;
const byte motor_drive_pinE = 3;
const byte STBY = 12; // standby
CDriveAxis *DriveAxis;

// приёмник ИК-сигнала
const byte ir_rc_pin = 11;
IRrecv irrecv(ir_rc_pin);
decode_results results;


CDriveControl DriveControl;
MovementDirection last_move_cmd = MMD_NONE;

CColorTracker ColorTracker;

long last_time = 0;

void setup()
{

  pinMode(2, OUTPUT); // servo rotation chassis
  pinMode(3, OUTPUT);// enable drive axis motor
  pinMode(4, OUTPUT);// pinA drive axis motor
  pinMode(5, OUTPUT);// pinB drive axis motor
  pinMode(STBY, OUTPUT); // 12 standby drive axis motor

  pinMode(13, OUTPUT); // led
  
  pinMode(14, INPUT);// pinA0 // in3 for motor2
  pinMode(15, INPUT);// pinA1 // switch(right) for чего-нибудь/ если его включить, то на А1 будет подаваться Vin (7.2В)



  Serial.begin(115200);

  ColorTracker = CColorTracker(S0, S1, S2, S3, color_out);
  
  DriveAxis = new CDriveAxis(motor_drive_pinA, motor_drive_pinE, motor_drive_pinB, STBY);
  DriveControl = CDriveControl(DriveAxis, servo_rotate_chassis); // TODO

  irrecv.enableIRIn();

  last_time = millis();
}

void loop()
{

  ColorTracker.GetColor();
  
//  ColorTracker.Calibrate();

//  IR_remote_recieve();

//  testing_movement();

//  move_by_line();
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

//считывание информации от ИК передатчика
void IR_remote_recieve()
{
//  irrecv.decode(&results);
    if (irrecv.decode(&results)) {
        Serial.print(results.value, BIN);
        Serial.print("\t bits of ");
        Serial.println(results.bits);
        irrecv.resume();// Receive the next value
    }
}
//
//const byte red_orange_threshold = 35; // R-component: orange < 35 < red
//const byte blue_white_threshold = 22; // B-component: white < 22 < blue
//const byte green_white_threshold = 45; // G-component: white < 45 < green
//
//void color_identifier()
//{
//  int redFrequency = 0;
//  int greenFrequency = 0;
//  int blueFrequency = 0;
//
//  // настраиваем датчик таким образом, чтобы считывать данные
//  // с фотодиодов с красным фильтром:
//  digitalWrite(S2,LOW);
//  digitalWrite(S3,LOW);
// 
//  // считываем выходную частоту:
//  redFrequency = pulseIn(color_out, LOW);
//
//  if (redFrequency < )
//  digitalWrite(13,HIGH);
//  
//}


// проверка исполнения команд движения
void testing_movement()
{
  DriveControl.turn_right();
  delay(500);

  DriveControl.move_forward();
  delay(2000);

  DriveControl.turn_left();
  delay(500);

  DriveControl.move_forward();
  delay(2000);
}
