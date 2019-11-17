#include "CColorTracker.h"

CColorTracker::CColorTracker(byte s0, byte s1, byte s2, byte s3, byte out)
{
  _color_out = out;
  _pin_s2 = s2;
  _pin_s3 = s3;
  pinMode(s3, OUTPUT); // 6 pin - S3 of color sensor
  pinMode(s2, OUTPUT); // 7 pin - S2 of color sensor
  pinMode(s1, OUTPUT); // 8 pin - S1 of color sensor
  pinMode(s0, OUTPUT); // 9 pin - S0 of color sensor
  pinMode(_color_out, INPUT); // 10 pin - Color OUT

  // задаем масштабирование частоты на 20%:
  digitalWrite(s0,HIGH);
  digitalWrite(s1,LOW);
}

void CColorTracker::Calibrate()
{
  // для хранения частоты, считанной фотодиодами
  int redFrequency = 0;
  int greenFrequency = 0;
  int blueFrequency = 0;

  // настраиваем датчик таким образом, чтобы считывать данные
  // с фотодиодов с красным фильтром:
  digitalWrite(_pin_s2, LOW);
  digitalWrite(_pin_s3, LOW);
 
  // считываем выходную частоту:
  redFrequency = pulseIn(_color_out, LOW);
 
   // печатаем данные от фотодиодов с красным фильтром:
  Serial.print("R = ");
  Serial.print(redFrequency);
  delay(100);
 
  // настраиваем датчик таким образом, чтобы считывать данные
  // с фотодиодов с зеленым фильтром:
//  digitalWrite(_pin_s2, HIGH);
//  digitalWrite(_pin_s2, HIGH);
 
  // считываем выходную частоту:
  greenFrequency = pulseIn(_color_out, LOW);
 
  // печатаем данные от фотодиодов с зеленым фильтром:
  Serial.print(" G = ");
  Serial.print(greenFrequency);
  delay(100);
 
  // настраиваем датчик таким образом, чтобы считывать данные
  // с фотодиодов с синим фильтром:
  digitalWrite(_pin_s2, LOW);
  digitalWrite(_pin_s3, HIGH);
 
  // считываем выходную частоту:
  blueFrequency = pulseIn(_color_out, LOW);
 
  // печатаем данные от фотодиодов с синим фильтром:
  Serial.print(" B = ");
  Serial.println(blueFrequency);
  delay(100);
}


EDefinedColor CColorTracker::GetColor()
{
  
  int greenFrequency = 0;
  int blueFrequency = 0;

  return DefineStartField();

  
}

EDefinedColor CColorTracker::DefineStartField()
{
  int redFrequency = 0;
  
  // настраиваем датчик таким образом, чтобы считывать данные
  // с фотодиодов с красным фильтром:
  digitalWrite(_pin_s2, LOW);
  digitalWrite(_pin_s2, LOW);
 
  // считываем выходную частоту:
  redFrequency = pulseIn(_color_out, LOW);

  if (redFrequency < red_orange_threshold)
  { 
    digitalWrite(13, LOW);
    return EDC_ORANGE;
  }
  else
  {
    digitalWrite(13, HIGH);
    return EDC_RED;
  }
}
