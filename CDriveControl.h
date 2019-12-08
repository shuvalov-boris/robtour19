#ifndef CDRIVECONTROL_H
#define CDRIVECONTROL_H

#include <Arduino.h>
#include <Servo.h>

#include "CGearMotor.h"

/// максимальное время, в течение которого робот поворачивается при перестроении на черную линию со следующей фишкой
#define MAX_TURN_TIME 1600 // [ms]
#define MIN_TURN_TIME 350 // [ms]

/// время, спустя которое после поворота начинать считать черные линии
#define TURN_TIME_CROSS_LINE 250 // [ms]

// коды выполнения движений (поворотов) для достижения следующей фишки (для метода loop)
#define DIRECTING_TURN 100
#define DIRECTING_TURN_OVER 101 // первый поворот окончен
#define COUNTING_BLACK_LINES 102 
#define COUNTED_BLACK_LINES 106 
#define FINAL_TURN_START 103
#define FINAL_TURN 104 // второй поворот выполняется
#define FINAL_TURN_OVER 105 // второй поворот окончен

#define SPEED 200 // forward   // 100
#define TURN_SPEED 200        //  80

#define SECOND_TURN_ANGLE_DECREASE_FACTOR 1.5 // 2.5 for 3->8


const byte TURN_DRIVE_ANGLE = 15;
const byte TURN_ANGLE = 40;
const byte FORWARD_ANGLE = 90; // угол сервопривода для езды прямо

// направление движения робота
enum EMovementDirection {
  EMD_NONE,  
  EMD_FORWARD, 
  EMD_TURN_LEFT, 
  EMD_TURN_RIGHT, 
  EMD_ROTATE_LEFT, 
  EMD_ROTATE_RIGHT, 
  EMD_BACKWARD,
  EMD_STOP
};


/// Осуществляет управление движением робота
/*!
  Реализует как простые функции движения (ехать прямо, повороты),
  так и алгоритм перемещения до следующей синей полосы со смещением
  на определенной количество черных полос всторону (алгоритм движения к фишке).
*/
class CDriveControl
{
public: 
  CDriveControl() {}
  
  /*!
    Конструктор, который устанавливает связь с ведущей задней осью и переднеми колесами на серво
    \param[in] ADriveAxis Объект класса управления мотором на ведущей оси
    \param[in] left_rot_pin Пин сервопривода левого переднего колеса робота
    \param[in] right_rot_pin Пин сервопривода правого переднего колеса робота
  */
  CDriveControl(CGearMotor *ADriveAxis, byte left_rot_pin, byte right_rot_pin);

  //! Ехать прямо вперёд
  void move_forward();
  
  //! Ехать назад
  void move_backward();
  
  //! Ехать прямо и налево
  /// \param[in] aangle угол в градусах, на который поворачивать передние колеса при повороте
  void turn_left(byte aangle = TURN_DRIVE_ANGLE);
  
  //! Ехать прямо и направо
  /// \param[in] aangle угол в градусах, на который поворачивать передние колеса при повороте
  void turn_right(byte aangle = TURN_DRIVE_ANGLE);

  /*!
    Команда движения к следующей фишке.
    \details Команда движения от одной цветной полосы (зоны) к следующей. Команда только инициирует движение робота. 
              Для полного выполнения алгоритма необходимо в цикле вызывать метод loop()
    \param[in] ADriveAxis Объект класса управления мотором на ведущей оси
    \param[in] left_rot_pin Пин сервопривода левого переднего колеса робота
    \param[in] right_rot_pin Пин сервопривода правого переднего колеса робота
    \warning Перед применением требуется настройка параметров в зависимости от развиваемой скорости.
    \todo Необходимо отладить алгоритм. Работает частично.
  */
  void move_counting_lines(int dist_in_lines);

  /// Движение робота к фишке
  /*!
     Необходимо вызывать в цикле до конца работы алгоритма.
     Движение контролируется на основе данных от датчиков линии.
     \param[in] leftEn Показание левого датчика линии: "0" - белое поле; "1" - черная линия
     \param[in] rightEn Показание правого датчика линии: "0" - белое поле; "1" - черная линия
     \return код завершения конректного этапа алгоритма, необходимый для перехода, например, к алгоритму движения по черной линии
   */
  byte loop(int leftEn, int rightEn);
  void search_lines(int leftEn, int rightIn);

private:
  CGearMotor *DriveAxis;
  byte left_rot_pin;
  byte right_rot_pin;
  Servo servol;
  Servo servor;

  // для небольших поворотов на 1-7 линий
  unsigned long maneuver_time;
  byte black_lines_count_to_pass;
  bool left_turn;
  
  // Маневр - совокупность движений по достижению следующей синей полосы (фишки)
  enum EManeuverStatus
  {
    EMS_DIRECTING_TURN,
    EMS_COUNTING_BLACK_LINES,
    EMS_FINAL_TURN,
    EMS_ALONG_BLACK_LINE // до синей полосы (или финиша)
  } Maneuver;

  // Этап прохождения роботом черной полосы на основе двух датчиков линии
  enum EBlackLinePassStatus
  {
    EBLPS_BOTH_EXPECTING,                         //    0 0
    EBLPS_FIRST_SENSOR_DETECTION,                 //    0 1
    EBLPS_FIRST_SENSOR_PASSED_SECOND_EXPECTING,   //    0 2
    EBLPS_BOTH_SENSORS_DETECTION,                 //    1 1
    EBLPS_FIRST_SENSOR_PASSED_SECOND_DETECTION,   //    1 2       
    EBLPS_BOTH_SENSORS_PASSED                     //    2 2
  } BlackLinePassStatus;
  
  byte passed_black_lines; // количество пересеченных черных линий (стартовая (1-ая) не считается)
  
  byte byOrder[2];

  int turn_time;
//  byte turn_add = 0;

  bool one_time_acceleration = false;

  void start_second_turn();
  
};

#endif // CDRIVECONTROL_H
