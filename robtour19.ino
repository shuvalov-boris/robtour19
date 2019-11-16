#include <IRremote.h>

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

long last_time = 0;

void setup()
{

  pinMode(2, OUTPUT); // servo rotation chassis
  pinMode(3, OUTPUT);// enable drive axis motor
  pinMode(4, OUTPUT);// pinA drive axis motor
  pinMode(5, OUTPUT);// pinB drive axis motor
  pinMode(STBY, OUTPUT); // 12 standby drive axis motor

//  pinMode(S3, OUTPUT); // 6 pin - S3 of color sensor
//  pinMode(S2, OUTPUT); // 7 pin - S2 of color sensor
//  pinMode(S1, OUTPUT); // 8 pin - S1 of color sensor
//  pinMode(S0, OUTPUT); // 9 pin - S0 of color sensor
//  pinMode(color_out, INPUT); // 10 pin - Color OUT
  
  pinMode(14, INPUT);// pinA0 // in3 for motor2
  pinMode(15, INPUT);// pinA1 // switch(right) for чего-нибудь/ если его включить, то на А1 будет подаваться Vin (7.2В)

// задаем масштабирование частоты на 20%:
//  digitalWrite(S0,HIGH);
//  digitalWrite(S1,LOW);
//  
//  digitalWrite(S2, HIGH);
//  digitalWrite(S3, HIGH);


  Serial.begin(115200);
  
  DriveAxis = new CDriveAxis(motor_drive_pinA, motor_drive_pinE, motor_drive_pinB, STBY);
  DriveControl = CDriveControl(DriveAxis, servo_rotate_chassis); // TODO

  irrecv.enableIRIn();

  last_time = millis();
}

void loop()
{
//  color_sensor_calibration();

  IR_remote_recieve();

<<<<<<< Updated upstream
  DriveControl.turn_right();
  delay(2000);

  DriveControl.move_forward();
  delay(2000);

  DriveControl.turn_left();
  delay(2000);

  DriveControl.move_forward();
  delay(2000);

=======
//  testing_movement();

//  move_by_line();
>>>>>>> Stashed changes
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
    if (irrecv.decode(&results)) {
        Serial.print(results.value, BIN);
        Serial.print("\t bits of ");
        Serial.println(results.bits);
        irrecv.resume();// Receive the next value
    }
}

void color_sensor_calibration()
{
  // для хранения частоты, считанной фотодиодами
  int redFrequency = 0;
  int greenFrequency = 0;
  int blueFrequency = 0;

  // настраиваем датчик таким образом, чтобы считывать данные
  // с фотодиодов с красным фильтром:
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
 
  // считываем выходную частоту:
  redFrequency = pulseIn(color_out, LOW);
 
   // печатаем данные от фотодиодов с красным фильтром:
  Serial.print("R = ");
  Serial.print(redFrequency);
  delay(100);
 
  // настраиваем датчик таким образом, чтобы считывать данные
  // с фотодиодов с зеленым фильтром:
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
 
  // считываем выходную частоту:
  greenFrequency = pulseIn(color_out, LOW);
 
  // печатаем данные от фотодиодов с зеленым фильтром:
  Serial.print(" G = ");
  Serial.print(greenFrequency);
  delay(100);
 
  // настраиваем датчик таким образом, чтобы считывать данные
  // с фотодиодов с синим фильтром:
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
 
  // считываем выходную частоту:
  blueFrequency = pulseIn(color_out, LOW);
 
  // печатаем данные от фотодиодов с синим фильтром:
  Serial.print(" B = ");
  Serial.println(blueFrequency);
  delay(100);
}

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
