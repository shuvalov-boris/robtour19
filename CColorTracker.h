#ifndef CCOLORTRACKER_H
#define CCOLORTRACKER_H

#include <Arduino.h>

// пороги определения цвета
#define red_orange_threshold  35 // R-component: orange < 35 < red

// BLUE THRESHOLDs: white 31-35 blue 42-56  black 75-88
#define blue_white_threshold  40 // B-component: white < 22 < blue
#define blue_black_threshold 61

// GREEN THRESHOLDs: white 31-35 green 42-56  black 75-88
#define green_white_threshold 45 // G-component: white < 45 < green
#define green_black_threshold 61 //

// black/white ?

// Количество подряд идущих измерений, соответствующих частоте конкретного цвета, чтобы считать измерения истинными.
// Параметр позволяет отбросить шумы датчика цвета.
#define CHAIN_SIZE 3

enum EDefinedColor
{
  EDC_UNKNOW,
  EDC_RED,
  EDC_ORANGE,
  EDC_BLUE,
  EDC_GREEN,
  EDC_YELLOW,
  EDC_WHITE,
  EDC_BLACK,
  EDC_ALL
};

/// Класс работы с датчиком цвета на TCS3200
/*!
    Отвечает за определение поля (1 или 2(!)) - распознавание цвета на старте,
    определение достижения синей полосы и зоны финиша (зеленого цвета)
    \todo Диапазон чувствительности у разных цветов датчика может быть разным, поэтому стоит их привести к общему знаменателю:
          greenColor = map(greenFrequency, 100, 199, 255, 0);
 */
class CColorTracker
{
public:
  CColorTracker() {}
  CColorTracker(byte s0, byte s1, byte s2, byte s3, byte out);

  void Calibrate(EDefinedColor color);
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
