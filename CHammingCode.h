#ifndef CHAMMINGCODE_H
#define CHAMMINGCODE_H

#include <IRremote.h>

int RECV_PIN = 11;
IRrecv irrecv0(RECV_PIN);
decode_results results0;
uint8_t rcv_counter = 0;
uint8_t received_data[8];
uint8_t correct_data[8];

class CHammingCode
{
public:
  CHammingCode();

  void loop();

private:
  byte Hamming74 (byte input);
};

#endif // CHAMMINGCODE_H
