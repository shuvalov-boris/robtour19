#include <IRremote.h>

#include "CColorTracker.h"
#include "CDriveAxis.h"
#include "CDriveControl.h"


// на каком этапе выполнения задачи находимся
enum ETaskPhase
{
  ETP_READY,
  ETP_START_FIELD,
  ETP_MOVING_TO_BLACK_LINE_OF_NEXT_COIN,
  ETP_MOVING_ALONG_BLACK_LINE_TO_NEXT_COIN,
  ETP_MOVING_TO_BLUE_LINE_1,
  ETP_BLUE_LINE_1_PASSED,
  ETP_BLUE_LINE_2,
  ETP_BLUE_LINE_3,
  ETP_FINISH_FIELD
};

const byte start_button_pin = A7;

// датчики линии
const byte left_line_follower_pin = A1;
const byte right_line_follower_pin = A0;

// датчики цвета
const byte S0 = 9;
const byte S1 = 8;
const byte S2 = 7;
const byte S3 = 17;
const byte color_out = 10;


// сервопривод передней оси поворота робота
const byte servo_rotate_left_wheel = 16;
const byte servo_rotate_right_wheel = 2;

// мотор ведущей оси
const byte motor_drive_pinA = 4;
const byte motor_drive_pinB = 5;
const byte motor_drive_pinE = 6;
const byte STBY = 12; // standby
CDriveAxis *DriveAxis;


CDriveControl DriveControl;
byte DCReturnedCode = 0;
EMovementDirection last_move_cmd = EMD_NONE;

CColorTracker ColorTracker;

int RECV_pin = 11;
IRrecv irrecv0(RECV_pin);
decode_results results0;
uint8_t rcv_counter = 0;
uint8_t received_data[8];
uint8_t correct_data[8];

bool data_is_read = false;

long last_time = 0;
const long TIME_TO_WAIT = 10000;

// for testing
byte passedMoveCount = 0;
byte CoinsPos[5] = {2, 6, 1, 8, 5};
byte Move = 0;



ETaskPhase TaskPhase = ETP_READY;

void setup()
{
  pinMode(1, OUTPUT); // servo left rotation axis
  pinMode(2, OUTPUT); // servo rotation chassis
  pinMode(6, OUTPUT); // enable drive axis motor
  pinMode(4, OUTPUT); // pinA drive axis motor
  pinMode(5, OUTPUT); // pinB drive axis motor
  pinMode(STBY, OUTPUT); // 12 standby drive axis motor

  pinMode(13, OUTPUT); // led
  
  pinMode(14, INPUT);// pinA0 // in3 for motor2
  pinMode(15, INPUT);// pinA1 // switch(right) for чего-нибудь/ если его включить, то на А1 будет подаваться Vin (7.2В)

  Serial.begin(115200);

  ColorTracker = CColorTracker(S0, S1, S2, S3, color_out);
  
  DriveAxis = new CDriveAxis(motor_drive_pinA, motor_drive_pinE, motor_drive_pinB, STBY);
  DriveControl = CDriveControl(DriveAxis, servo_rotate_left_wheel, servo_rotate_right_wheel); // TODO

  irrecv0.enableIRIn(); // Start the receiver

  last_time = millis();
}

void loop()
{
//DriveControl.move_forward();
  move_by_line( );
//
////  testing_movement();
  return;
//
//    if (millis() - last_time < 3000)
//        DriveControl.turn_right();
//     else
//     DriveControl.turn_left();
//
//     return;
//       
//  ColorTracker.GetColor();

//  Считываем данные о положении фишек и роботов
//  Определяем по датчику цвета номер поля - 1 или 2

    if (TaskPhase == ETP_READY)
    {
      hamming_loop();
      if (millis() - last_time > TIME_TO_WAIT)
    
    {
       TaskPhase = ETP_START_FIELD;
      Serial.println("EMERGE FORWARD");
    }
    }
      

  
    switch (TaskPhase)
    {
      case ETP_READY:
        //  Считываем данные о положении фишек и роботов
        if (data_is_read == false)
        { 
          hamming_loop();
        }
        if (data_is_read == true)
        {
          Serial.println("READ IR REMOTE DATA");
          // if цвет стартовой зоны определен
          // задаем массив CoinsPos
          //  Определяем по датчику цвета номер поля - 1 или 2
          EDefinedColor color = ColorTracker.GetColor();
          if (color == EDC_YELLOW)
          {
            Serial.println("YELLOW");
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
            Serial.println("RED");
            for (int i = 0; i < 4; i++)
            {
              CoinsPos[i] = correct_data[i + 4];
              Serial.print(CoinsPos[i]);
              Serial.print("\t");
            }
            Serial.println();
          }
          CoinsPos[4] = 5; // or 4
        }   
        
        //  if НАЖАТА КНОПКА СТАРТА то
//          TaskPhase = ETP_START_FIELD;

// ВРЕМЕННО !!!  ВРЕМЕННО !!!  ВРЕМЕННО !!!  ВРЕМЕННО !!!  ВРЕМЕННО !!!  ВРЕМЕННО !!!  ВРЕМЕННО !!!  ВРЕМЕННО !!!  

          if (data_is_read == true){
            TaskPhase = ETP_START_FIELD;
            Serial.println("START");
          }
        break;
      case ETP_START_FIELD:
        DriveControl.move_counting_lines(CoinsPos[passedMoveCount + 1] - CoinsPos[passedMoveCount]);
        TaskPhase = ETP_MOVING_TO_BLACK_LINE_OF_NEXT_COIN;
        break;

      case ETP_MOVING_TO_BLACK_LINE_OF_NEXT_COIN:
        int left = digitalRead(left_line_follower_pin);
        int right = digitalRead(right_line_follower_pin);
        if (DriveControl.loop(left, right) == FINAL_TURN_OVER)
        {
          TaskPhase = ETP_MOVING_ALONG_BLACK_LINE_TO_NEXT_COIN;
          move_by_line();
        }
        break;
      case ETP_MOVING_ALONG_BLACK_LINE_TO_NEXT_COIN:
        move_by_line();
        if (passedMoveCount < 3 && ColorTracker.GetColor() == EDC_BLUE)
        {
          passedMoveCount++;
          TaskPhase = ETP_START_FIELD;
          if (passedMoveCount == 3)
          {
            // Можно поднять платформу (сначала подобрав монетки)
          }
        }
        else if (passedMoveCount == 3 && ColorTracker.GetColor() == EDC_GREEN)
        {
          // На финише. Поднять платформу! раньше надо было
        }
        break;
    }
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
    last_move_cmd = EMD_FORWARD;
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
    if (last_move_cmd == EMD_TURN_RIGHT)
    {
      DriveControl.turn_right(); 
      last_move_cmd = EMD_TURN_RIGHT;
//      Serial.println("repeat RIGHT");
//      confused_threshold++;
    }
    else if (last_move_cmd == EMD_TURN_LEFT)
    {
      DriveControl.turn_left(); 
      last_move_cmd = EMD_TURN_LEFT;
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



void hamming_loop()
{
  if (irrecv0.decode(&results0)) {
    if (rcv_counter == 0) {
      if (results0.value == 0xffffffff) {
        rcv_counter = 1;
      }
    } else {      
      if (rcv_counter == 1) {
        received_data[0] = (uint8_t)(results0.value >> 24);
        correct_data[0] = Hamming74(received_data[0]);
        received_data[1] = (uint8_t)(results0.value >> 16);
        correct_data[1] = Hamming74(received_data[1]);
        received_data[2] = (uint8_t)(results0.value >> 8);
        correct_data[2] = Hamming74(received_data[2]);
        received_data[3] = (uint8_t)(results0.value);
        correct_data[3] = Hamming74(received_data[3]);
      } else if (rcv_counter == 2) {
        received_data[4] = (uint8_t)(results0.value >> 24);
        correct_data[4] = Hamming74(received_data[4]);
        received_data[5] = (uint8_t)(results0.value >> 16);
        correct_data[5] = Hamming74(received_data[5]);
        received_data[6] = (uint8_t)(results0.value >> 8);
        correct_data[6] = Hamming74(received_data[6]);
        received_data[7] = (uint8_t)(results0.value); 
        correct_data[7] = Hamming74(received_data[7]);       
      }
      rcv_counter ++;
    }
    if(rcv_counter > 2) {
      rcv_counter = 0;
      
//      Serial.print("Received data:");
//      for (int i = 0; i < 8; i++) {
//        Serial.print(" ");
//        Serial.print(received_data[i]);
//      }
//      Serial.println();
//
      Serial.print("Correct data:");
      for (int i = 0; i < 8; i++) {
        Serial.print(" ");
        Serial.print(correct_data[i]);
      }
      Serial.println();
      data_is_read = true;
    }
    irrecv0.resume(); // Receive the next value
  }
}

byte Hamming74 (byte input)
{

  //узнаем значения битов   
  boolean
  bit8=(0 < (input & B00000001)), // data
  bit7=(0 < (input & B00000010)), // data
  bit6=(0 < (input & B00000100)), // data
  bit5=(0 < (input & B00001000)), 
  bit4=(0 < (input & B00010000)), // data
  bit3=(0 < (input & B00100000)),
  bit2=(0 < (input & B01000000)),
  bit1=(0 < (input & B10000000));

  //  делаем XOR к трём проверочным битам 
  boolean
  SumCheck1 = (bit4 ^ bit6 ^ bit8),
  SumCheck2 = (bit4 ^ bit7 ^ bit8),
  SumCheck3 = (bit6 ^ bit7 ^ bit8);

  

  //  сравниваем значение XOR проверочного бита и бита используемого в посылке
  if (!((SumCheck1 == bit1) && (SumCheck2 == bit2) && (SumCheck3 == bit4)))
  {    
    boolean
      SumCheckSyndrome1 = (bit2 ^ bit4 ^ bit6 ^ bit8),
      SumCheckSyndrome2 = (bit3 ^ bit4 ^ bit7 ^ bit8),
      SumCheckSyndrome3 = (bit5 ^ bit6 ^ bit7 ^ bit8);
  
     byte SyndromeSumCheck = 1;
     if (SumCheckSyndrome1 == 1) { SyndromeSumCheck+=1; }
     if (SumCheckSyndrome2 == 1) { SyndromeSumCheck+=2; }
     if (SumCheckSyndrome3 == 1) { SyndromeSumCheck+=4; } 

    if (SyndromeSumCheck > 0)
    { // исправляем ошибку в бите данных
      if (SyndromeSumCheck == 4) bit4 = 1 - bit4;
      if (SyndromeSumCheck == 6) bit6 = 1 - bit6;
      if (SyndromeSumCheck == 7) bit7 = 1 - bit7;
      if (SyndromeSumCheck == 8) bit8 = 1 - bit8;
    }
  }


   byte result = 0;
    if (bit8==1) {result+=1;}  else {result+=0;}
    if (bit7==1) {result+=2;}  else {result+=0;}
    if (bit6==1) {result+=4;}  else {result+=0;}
    if (bit4==1) {result+=8;}  else {result+=0;}

    return result;
}
