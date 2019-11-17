#ifndef CCOLORTRACKER_H
#define CCOLORTRACKER_H

#include "Arduino.h"

// пороги определения цвета
#define red_orange_threshold  35 // R-component: orange < 35 < red
#define blue_white_threshold  22 // B-component: white < 22 < blue
#define green_white_threshold 45 // G-component: white < 45 < green
// black/white ?

enum EDefinedColor
{
  EDC_UNKNOW,
  EDC_RED,
  EDC_ORANGE,
  EDC_BLUE,
  EDC_GREEN,
  EDC_YELLOW
};

// Отвечает за определение поля (1 или 2(!)) - распознавание цвета на старте,
// определение достижения синей полосы и зоны финиша (зеленого цвета)
class CColorTracker
{
public:
  CColorTracker() {}
  CColorTracker(byte s0, byte s1, byte s2, byte s3, byte out);

  void Calibrate();
  void DefineColorNow(EDefinedColor AColor) {} // использовать или нет?

  EDefinedColor GetColor();

private:
  byte _pin_s2;
  byte _pin_s3;
  byte _color_out;

  EDefinedColor DefineStartField();
};

#endif // CCOLORTRACKER_H
