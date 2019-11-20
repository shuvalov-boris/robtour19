#include "CHammingCode.h"

CHammingCode::CHammingCode(byte recv_pin)
{
  irrecv = new IRrecv(recv_pin);
  irrecv->enableIRIn(); // Start the receiver 
}

bool CHammingCode::ReceiveData(uint8_t * data)
{
#ifndef IK_RECV_SIMULATION
  if (irrecv->decode(&results)) {
    if (rcv_counter == 0) {
      if (results.value == 0xffffffff) {
        rcv_counter = 1;
      }
    } else {      
      if (rcv_counter == 1) {
        received_data[0] = (uint8_t)(results.value >> 24);
        received_data[1] = (uint8_t)(results.value >> 16);
        received_data[2] = (uint8_t)(results.value >> 8);
        received_data[3] = (uint8_t)(results.value);
      } else if (rcv_counter == 2) {
        received_data[4] = (uint8_t)(results.value >> 24);
        received_data[5] = (uint8_t)(results.value >> 16);
        received_data[6] = (uint8_t)(results.value >> 8);
        received_data[7] = (uint8_t)(results.value); 
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

      data = CorrectMessage(received_data);

//      Serial.print("Correct data:");
//      for (int i = 0; i < 8; i++) {
//        Serial.print(" ");
//        Serial.print(data[i]);
//      }
//      Serial.println();
    }

    irrecv->resume(); // Receive the next value
    return (rcv_counter > 2);
  }  
#else
      data = SIMULATION_DATA;
      return true;
#endif
}

bool CHammingCode::test()
{
  uint8_t input[MESSAGE_SIZE] = {67, 42, 53, 103, 76, 15, 96, 104};
  uint8_t output[MESSAGE_SIZE] = {3, 2, 5, 6, 4, 7, 8, 1};

  uint8_t * correct_data = CorrectMessage(input);

  for (int i = 0; i < MESSAGE_SIZE; ++i)
    if (correct_data[i] != output[i])
      return false;
  return true;
}

uint8_t * CHammingCode::CorrectMessage(uint8_t * raw_data)
{
  uint8_t correct_data[MESSAGE_SIZE];
  for (int i = 0; i < MESSAGE_SIZE; ++i)
    correct_data[i] = Hamming74(raw_data[i]);
  return correct_data;
}

byte CHammingCode::Hamming74 (uint8_t input)
{

  //узнаем значения битов   
  boolean
    bit8 = (0 < (input & B00000001)), // data
    bit7 = (0 < (input & B00000010)), // data
    bit6 = (0 < (input & B00000100)), // data
    bit5 = (0 < (input & B00001000)), 
    bit4 = (0 < (input & B00010000)), // data
    bit3 = (0 < (input & B00100000)),
    bit2 = (0 < (input & B01000000)),
    bit1 = (0 < (input & B10000000));

  //  делаем XOR к трём проверочным битам 
  boolean
    SumCheck1 = (bit4 ^ bit6 ^ bit8),
    SumCheck2 = (bit4 ^ bit7 ^ bit8),
    SumCheck3 = (bit6 ^ bit7 ^ bit8);

  //  сравниваем значение XOR проверочного бита и бита используемого в посылке
  if (!((SumCheck1 == bit1) && (SumCheck2 == bit2) && (SumCheck3 == bit4)))
  {    
    boolean
      SumCheckSyndrome1 = (bit2 ^ SumCheck1),
      SumCheckSyndrome2 = (bit3 ^ SumCheck2),
      SumCheckSyndrome3 = (bit5 ^ SumCheck3);
  
    byte SyndromeSumCheck = 1;
      if (SumCheckSyndrome1 == 1) SyndromeSumCheck += 1;
      if (SumCheckSyndrome2 == 1) SyndromeSumCheck += 2;
      if (SumCheckSyndrome3 == 1) SyndromeSumCheck += 4;

    if (SyndromeSumCheck > 0)
    { // исправляем ошибку в бите данных
      if (SyndromeSumCheck == 4) bit4 = 1 - bit4;
      if (SyndromeSumCheck == 6) bit6 = 1 - bit6;
      if (SyndromeSumCheck == 7) bit7 = 1 - bit7;
      if (SyndromeSumCheck == 8) bit8 = 1 - bit8;
    }
  }
  
  byte result = 0;
  if (bit8 == 1) result += 1;
  if (bit7 == 1) result += 2;
  if (bit6 == 1) result += 4;
  if (bit4 == 1) result += 8;

  return result;
}
