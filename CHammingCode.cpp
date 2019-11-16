#include "CHammingCode.h"

CHammingCode::CHammingCode()
{
  irrecv.enableIRIn(); // Start the receiver
}

void CHammingCode::loop()
{
  if (irrecv.decode(&results)) {
    if (rcv_counter == 0) {
      if (results.value == 0xffffffff) {
        rcv_counter = 1;
      }
    } else {      
      if (rcv_counter == 1) {
        received_data[0] = (uint8_t)(results.value >> 24);
        correct_data[0] = Hamming74(received_data[0]);
        received_data[1] = (uint8_t)(results.value >> 16);
        correct_data[1] = Hamming74(received_data[1]);
        received_data[2] = (uint8_t)(results.value >> 8);
        correct_data[2] = Hamming74(received_data[2]);
        received_data[3] = (uint8_t)(results.value);
        correct_data[3] = Hamming74(received_data[3]);
      } else if (rcv_counter == 2) {
        received_data[4] = (uint8_t)(results.value >> 24);
        correct_data[4] = Hamming74(received_data[4]);
        received_data[5] = (uint8_t)(results.value >> 16);
        correct_data[5] = Hamming74(received_data[5]);
        received_data[6] = (uint8_t)(results.value >> 8);
        correct_data[6] = Hamming74(received_data[6]);
        received_data[7] = (uint8_t)(results.value); 
        correct_data[7] = Hamming74(received_data[7]);       
      }
      rcv_counter ++;
    }
    if(rcv_counter > 2) {
      rcv_counter = 0;
      Serial.print("Received data:");
      for (int i = 0; i < 8; i++) {
        Serial.print(" ");
        Serial.print(received_data[i]);
      }
      Serial.println();

      Serial.print("Correct data:");
      for (int i = 0; i < 8; i++) {
        Serial.print(" ");
        Serial.print(correct_data[i]);
      }
      Serial.println();
    }
    irrecv.resume(); // Receive the next value
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
