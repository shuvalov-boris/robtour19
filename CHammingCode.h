#pragma once

#include "ARduino.h"
#include "IRremote.h"

#define IK_RECV_SIMULATION // comment for real data receiving

#define MESSAGE_SIZE 8

const byte SIMULATION_DATA[MESSAGE_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8};

//Считывает положение фишек
class CHammingCode
{
public:
  CHammingCode(byte recv_pin);

  // Чтобы получить сообщение, необходимо вызывать ReceiveData до тех пор, пока она не вернет true
  // TODO реализовать условие достоверности сообщения последовательным получении одних и тех же данных
  bool CHammingCode::ReceiveData(uint8_t * data);

  bool test();

private:
  // расшифровка сообщения
  uint8_t * CHammingCode::CorrectMessage(uint8_t * raw_data);
  
  // расшифровывкапринятого байта с учетом ошибочного бита
  byte Hamming74 (uint8_t input);

  IRrecv *irrecv;
  decode_results results;
  
  uint8_t rcv_counter = 0; // 1 - получены первые 4 байта; 2 - вторые 4 байта
  uint8_t received_data[MESSAGE_SIZE];
};
