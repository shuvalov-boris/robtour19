#ifndef CCOLORTRACKER_H
#define CCOLORTRACKER_H

#include <Arduino.h>

// пороги определения цвета
#define red_orange_threshold  35 // R-component: orange < 35 < red
#define blue_white_threshold  35 // B-component: white < 22 < blue
#define blue_black_threshold 45
#define green_white_threshold 45 // G-component: white < 45 < green
// black/white ?

enum EDefinedColor
{
  EDC_UNKNOW,
  EDC_RED,
  EDC_ORANGE,
  EDC_BLUE,
  EDC_GREEN,
  EDC_YELLOW,
  EDC_WHITE,
  EDC_BLACK
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

  EDefinedColor GetColor(EDefinedColor color);
  EDefinedColor DefineStartField();

private:
  byte _pin_s2;
  byte _pin_s3;
  byte _color_out;

  byte value;
  byte count;

  
};

#endif // CCOLORTRACKER_H
