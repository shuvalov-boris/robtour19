/**@file robtour19.ino */

#include "CColorTracker.h"
#include "CDriveControl.h"
#include "CGearMotor.h"
#include "CHammingCode.h"

// задержка на срабатывание книпки старта
#define BTN_TIME 1000 // [ms]

// порог аналогово-цифрового преобразования для датчика черной полосы
#define LF_SENSOR_THRESHOLD 900

// на каком этапе выполнения задачи находимся
enum ETaskPhase
{
  ETP_READY,
  ETP_START_FIELD,
  ETP_MOVING_TO_BLACK_LINE_OF_NEXT_COIN,
  ETP_MOVING_ALONG_BLACK_LINE_TO_NEXT_COIN,
  ETP_FINISH_FIELD
};

// Стадия сбора
enum ECoinsLiftPhase
{
  ECLP_COLLECTING, // сбор фишек
  ECLP_GRIPPING,   // захват фишек
  ECLP_LIFTING,    // подъём и поворот вверх лапы с фишками
  ECLP_TOWER_TILT,
  ECLP_COMPLETED
};

const byte start_button_pin = A7;

// датчики линии
const byte left_line_follower_pin = A1;
const byte right_line_follower_pin = A6;

// датчики цвета
const byte S0 = 16; // A2
const byte S1 = 18; // A4
const byte S2 = 19; // A5
const byte S3 = 17;
const byte color_out = 10;


// сервопривод передней оси поворота робота
//const byte servo_rotate_left_wheel = 16;
const byte servo_rotate_right_wheel = 2;

// сервопривод управления лапой (платформа с липучкой для сбора фишек)
const byte servo_gripper_pin = A0;
Servo servo_gripper; //TODO init

// сервопривод управления натяжением верёвки для подъёма лапы
const byte servo_rope_tension_pin = 123;
Servo servo_rope_tension; //TODO init

// наклон башни
const byte servo_tower_tilt_pin = 13;
Servo servo_tower_tilt; //TODO

const byte motor_tower_pinA = 7;
const byte motor_tower_pinB = 8;
const byte motor_tower_pinE = 9;
//const byte STBY = 12; // standby
CGearMotor TowerMotor; // Мотор для подъема платформы с фишками

// мотор ведущей оси
const byte motor_drive_pinA = 4;
const byte motor_drive_pinB = 5;
const byte motor_drive_pinE = 6;
const byte STBY = 12; // standby
CGearMotor *DriveAxis;


CDriveControl DriveControl;
byte DCReturnedCode = 0;
EMovementDirection last_move_cmd = EMD_NONE;

CColorTracker ColorTracker;

int RECV_pin = 11;
CHammingCode HammingCode(RECV_pin);

bool data_is_read = false;

long last_time = 0;
const long TIME_TO_WAIT = 10000;

// for testing
byte passedMoveCount = 0;
byte CoinsPos[5] = {2, 6, 1, 8, 5};
byte Move = 0;

bool is_setup = false;


ETaskPhase TaskPhase = ETP_READY;

void setup()
{
  pinMode(1, OUTPUT); // servo left rotation axis
  pinMode(2, OUTPUT); // servo rotation chassis
  pinMode(3, INPUT_PULLUP); // кнопка старта
  pinMode(4, OUTPUT); // pinA drive axis motor
  pinMode(5, OUTPUT); // pinB drive axis motor TOWER
  pinMode(6, OUTPUT); // enable drive axis motor
  
  pinMode(8, OUTPUT); // pinA drive axis motor
  pinMode(9, OUTPUT); // pinB drive axis motor TOWER
  pinMode(STBY, OUTPUT); // 12 standby drive axis motor

  pinMode(13, OUTPUT); // servo tower
  
  pinMode(14, INPUT);// pinA0 // in3 for motor2
  pinMode(15, INPUT);// pinA1 // switch(right) for чего-нибудь/ если его включить, то на А1 будет подаваться Vin (7.2В)

  pinMode(A6, INPUT);
  Serial.begin(115200);

  ColorTracker = CColorTracker(S0, S1, S2, S3, color_out);
  
  DriveAxis = new CGearMotor(motor_drive_pinA, motor_drive_pinE, motor_drive_pinB, STBY);
  DriveControl = CDriveControl(DriveAxis, servo_rotate_right_wheel, servo_rotate_right_wheel); // TODO rm left servo

  TowerMotor = CGearMotor(motor_tower_pinA, motor_tower_pinE, motor_tower_pinB, STBY);

//  Serial.print("testing HammingCode...\t\t");
//  bool is_correct = HammingCode.test();
//  if (is_correct == true) Serial.println("OK");
//  else Serial.println("FAIL");

  servo_tower_tilt.attach(servo_tower_tilt_pin);
  servo_tower_tilt.write(0);

  servo_gripper.attach(servo_gripper_pin);
  servo_gripper.write(0);

//  TowerMotor.setSpeed(255);
//  TowerMotor.forward();
//  delay(50);
//  TowerMotor.stop();
//
//  DriveAxis->setSpeed(255);
//  DriveAxis->forward();
//  delay(100);
//  DriveAxis->stop();

  last_time = millis();
}

int tower_angle = 0;
int gripper_angle = 0;
int last_btn_state = HIGH;
int incomingByte = 0;

void loop()
{

//int rightLF = analogRead(right_line_follower_pin);
//  if (rightLF > LF_SENSOR_THRESHOLD)
//    rightLF = HIGH;
//  else
//    rightLF = LOW;
//Serial.println(rightLF);
//
//DriveAxis->setSpeed(255);
//DriveAxis->forward();

//  ColorTracker.Calibrate(EDC_BLUE);

//  проверка подсчета черных линий
//  int left = digitalRead(left_line_follower_pin);
//  int right = digitalRead(right_line_follower_pin);
//  DriveControl.search_lines(left, right);

//servo_gripper.write(5);

//  collect_coins();


// testing of tower     
//    if (digitalRead(3) == LOW && last_btn_state == HIGH){
//      servo_lip.write(++tower_angle);
//      delay(100);
//      last_btn_state = LOW;
//      Serial.println(tower_angle);
//    }
//    else
//      last_btn_state = HIGH;



  if (Serial.available() > 0)
  {
    incomingByte = Serial.read();

    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);   
    Serial.read(); 
  }

     if (tower_angle < 20 && incomingByte == 50)
    {
      servo_tower_tilt.write(++tower_angle);
      delay(50);
//      last_btn_state = LOW;
      Serial.println(tower_angle);
      Serial.read();
    }

    if (tower_angle < 40 && incomingByte == 51)
    {
      servo_tower_tilt.write(++tower_angle);
      delay(100);
//      last_btn_state = LOW;
      Serial.println(tower_angle);
      Serial.read();
    }

    if (tower_angle < 60 && incomingByte == 52)
    {
      servo_tower_tilt.write(++tower_angle);
      delay(100);
//      last_btn_state = LOW;
      Serial.println(tower_angle);
      Serial.read();
    }

    
    if (tower_angle < 80 && incomingByte == 53)
    {
      servo_tower_tilt.write(++tower_angle);
      delay(100);
//      last_btn_state = LOW;
      Serial.println(tower_angle);
      Serial.read();
    }

    
    if (tower_angle < 90 && incomingByte == 54)
    {
      servo_tower_tilt.write(++tower_angle);
      delay(100);
//      last_btn_state = LOW;
      Serial.println(tower_angle);
      Serial.read();
    }

    
    if (incomingByte == 119)
    {
      servo_tower_tilt.write(++tower_angle);
      delay(100);
//      last_btn_state = LOW;
      Serial.println(tower_angle);
      Serial.read();
      incomingByte = 0;
    }

    if (tower_angle > 0 && incomingByte == 55)
    {
      servo_tower_tilt.write(--tower_angle);
      delay(100);
//      last_btn_state = LOW;
      Serial.println(tower_angle);
    }

    if (gripper_angle < 100 && incomingByte == 117) // lift up [u]
    {
      servo_gripper.write(++gripper_angle);
      delay(10);
//      last_btn_state = LOW;
      Serial.println(gripper_angle);
      Serial.read();
    }

    if (gripper_angle > 0 && incomingByte == 100) // lift down [d]
    {
      servo_gripper.write(--gripper_angle);
      delay(10);
//      last_btn_state = LOW;
      Serial.println(gripper_angle);
      Serial.read();
    }
}

// выполнение задачи сбора фишек
void collect_coins()
{
//  Serial.print("TaskPhase is ");
//  Serial.println((int)TaskPhase);
//  switch (TaskPhase)
//  {
  if (TaskPhase == ETP_READY)
  {
    //  Считываем данные о положении фишек и роботов
    uint8_t correct_data[] = {5, 6, 8, 4, 5, 6, 4, 3};
    data_is_read = true;
    if (data_is_read == false)
    {
      data_is_read = HammingCode.ReceiveData(correct_data);
    }
    if (data_is_read == true && is_setup == false)
    {
      Serial.print("IR Received data:");
      for (int i = 0; i < 8; i++) {
        Serial.print(" ");
        Serial.print(correct_data[i]);
      }
      Serial.println();
      
      Serial.println("READ IR REMOTE DATA");
      // if цвет стартовой зоны определен
      // задаем массив CoinsPos
      //  Определяем по датчику цвета номер поля - 1 или 2
      EDefinedColor color = ColorTracker.DefineStartField();
      if (color == EDC_YELLOW)
      {
        Serial.println("YELLOW start field");
        for (int i = 0; i < 4; i++)
        {
          CoinsPos[i] = correct_data[i];
          Serial.print(CoinsPos[i]);
          Serial.print("\t");
        }
        Serial.println();
      }
      else if (color == EDC_RED)
      {
        Serial.println("RED start field");
        for (int i = 0; i < 4; i++)
        {
          CoinsPos[i] = correct_data[i + 4];
          Serial.print(CoinsPos[i]);
          Serial.print("\t");
        }
        Serial.println();
      }
      else
      {
        Serial.print("got color "); 
        Serial.println(color);
      }
      Serial.println();
      CoinsPos[4] = 5; // or 4 чтобы на финише быть под центром мишени
      is_setup = true;
    }   
     
    if (digitalRead(3) == LOW){
      delay(BTN_TIME);
      TaskPhase = ETP_START_FIELD;
      Serial.println("START");
    }
// ------------------/\---------------------- \\
//    break;
    return;
  }
  
  else if (TaskPhase == ETP_START_FIELD)
  {
    DriveControl.move_counting_lines(CoinsPos[passedMoveCount + 1] - CoinsPos[passedMoveCount]);
    TaskPhase = ETP_MOVING_TO_BLACK_LINE_OF_NEXT_COIN;
    Serial.print("I'm at position #");
    Serial.print(CoinsPos[passedMoveCount]);
    Serial.print(" and head to line #");
    Serial.println(CoinsPos[passedMoveCount + 1]); 
    return;
  }
  else if (TaskPhase == ETP_MOVING_TO_BLACK_LINE_OF_NEXT_COIN)
  {
//    Serial.println("ETP_MOVING_TO_BLACK_LINE_OF_NEXT_COIN");
    int leftLF = digitalRead(left_line_follower_pin);
    int rightLF = analogRead(right_line_follower_pin);
    if (rightLF > LF_SENSOR_THRESHOLD)
      rightLF = HIGH;
    else
      rightLF = LOW;
    if (DriveControl.loop(leftLF, rightLF) == FINAL_TURN_OVER)
    {
      Serial.println("Got FINAL_TURN_OVER return code");
      TaskPhase = ETP_MOVING_ALONG_BLACK_LINE_TO_NEXT_COIN;
      move_by_line();
    }
    return;
  }
  else if (TaskPhase == ETP_MOVING_ALONG_BLACK_LINE_TO_NEXT_COIN)
  {
    move_by_line();
//    Serial.println("Searching BLUE");
// TODO Может начинать искать синий еще при завершающем повороте (для нашего тестового поля с короткими расстояниями)
    if ((passedMoveCount < 3) && (ColorTracker.GetColor(EDC_BLUE) == EDC_BLUE))
    {
      Serial.println("REACH BLUE");
      Serial.println();
      ColorTracker.Clear();
//      DriveAxis->stop();
//      delay(2000);
      passedMoveCount++;
      TaskPhase = ETP_START_FIELD;
      if (passedMoveCount == 3)
      {
        TaskPhase = ETP_FINISH_FIELD; // ВРЕМЕННО !!! убрать на полноценном поле!
        Serial.println("GOT ALL COINS");
        // Захватить фишки
        // Поднять платформу
        // Повернуть платформу
        // Можно поднять платформу (сначала подобрав монетки)
        TowerControl();
      }
    }
    else if ((passedMoveCount >= 3) && (ColorTracker.GetColor(EDC_GREEN) == EDC_GREEN))
    {
      Serial.println("FINISH");
      TaskPhase = ETP_FINISH_FIELD;
      // На финише. Поднять платформу! раньше надо было
    }
//    break;
    return;
  }

  else if (TaskPhase == ETP_FINISH_FIELD)
  {
    delay(1000);
    DriveAxis->stop();
    return;
  }
}

void TowerControl()
{
  // Захватить фишки
  // Поднять платформу
  // Повернуть платформу
}



// движение по черной полосе
// Можно вынести в класс, передавая ссылку на объект управления движением
void move_by_line()
{
  int leftLF = digitalRead(left_line_follower_pin);
  int rightLF = analogRead(right_line_follower_pin);
  if (rightLF > LF_SENSOR_THRESHOLD)
    rightLF = HIGH;
  else
    rightLF = LOW;

//  Serial.print(leftLF);
//  Serial.print("\t");
//  Serial.println(rightLF);

  if (leftLF == LOW && rightLF == LOW)
  {
    DriveControl.move_forward();
    last_move_cmd = EMD_FORWARD;
//    Serial.println("to FORWARD");
  }
  else if (leftLF == HIGH && rightLF == HIGH)
  {
    if (last_move_cmd == EMD_TURN_RIGHT)
    {
      DriveControl.turn_right(); 
      last_move_cmd = EMD_TURN_RIGHT;
//      Serial.println("repeat RIGHT");
    }
    else if (last_move_cmd == EMD_TURN_LEFT)
    {
      DriveControl.turn_left(); 
      last_move_cmd = EMD_TURN_LEFT;
//      Serial.println("repeat LEFT");
    }
    else
    { // произошло нечто неординарное
//      Serial.println("ЧЕРНАЯ ПОЛОСА ПОПЕРЕК");
      DriveControl.move_forward();
      last_move_cmd = EMD_FORWARD;
    }
  }
  else if (leftLF == LOW && rightLF == HIGH)
  {
    DriveControl.turn_right();
    last_move_cmd = EMD_TURN_RIGHT;
//    Serial.println("to RIGHT");
  }
  else if (leftLF == HIGH && rightLF == LOW)
  {
    DriveControl.turn_left();
    last_move_cmd = EMD_TURN_LEFT;
//    Serial.println("to LEFT");
  }
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
