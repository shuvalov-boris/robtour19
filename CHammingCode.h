#pragma once

#include <Arduino.h>
#include <IRremote.h>

#define IK_RECV_SIMULATION // comment for real data receiving

#define MESSAGE_SIZE 8

const byte SIMULATION_DATA[MESSAGE_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8};

/// Принимает сообщение (координаты роботов и фишек на поле) от ИК передатчика
/*!
  Также позволяет имитировать прием информации при отсутствии реального передатчика. Для этого определяется идентификатор IK_RECV_SIMULATION.
  Для работы класса в режиме реального приема данных необходимо закомментировать строку с объявлением IK_RECV_SIMULATION.
*/
class CHammingCode
{
public:
  /*!
    Конструктор, который устанавливает пин ИК приёмника
    \param[in] recv_pin Пин ИК приёмника
  */
  CHammingCode(byte recv_pin);

  ///Принимает положение фишек и роботов от ИК передатчика
  /*! 
    Чтобы получить сообщение, необходимо вызывать ReceiveData до тех пор, пока он не вернет значение true.
      
    Формат сообщения - MESSAGE_SIZE чисел от 1 до 8:
      -положение первого робота на первом поле;
      -положение фишки на первой полосе первого поля;
      -положение фишки на второй полосе первого поля;
      -положение фишки на третей полосе первого поля;
      -положение второго робота на втором поле;
      -положение фишки на первой полосе второго поля;
      -положение фишки на второй полосе второго поля;
      -положение фишки на третей полосе второго поля;
    \param[out] data сообщение (массив из 8 чисел) в случае успешного получения и декодирования сообщения 
    \return true, если сообщение получено и декодировано; в противном случае - false.
    \warning Полученные данные не проверяются на наличие помех (из-за слабого сигнала, например)
    \todo Необходимо реализовать условие достоверности сообщения проверкой последовательного получения одних и тех же данных
  */
  bool CHammingCode::ReceiveData(uint8_t * data);

  /// проверка алгоритма декодирования и корректировки сообщения на тестовых данных
  bool test();

private:
  /// Декодирование сообщения
  /*!
    Декодирует сообщение, закодированное кодом Хэмминга длины MESSAGE_SIZE байт
    \param[in] Сообщение, закодированное при помощи кода Хэмминга длины MESSAGE_SIZE байт
    \return Декодированное сообщение, содержащее MESSAGE_SIZE чисел
   */
  uint8_t * CHammingCode::CorrectMessage(uint8_t * raw_data);
  
  /// Декодирование принятого байта с учетом ошибочного бита
  /*!
    Декодирует байт, закодированный кодом Хэмминга
    \param[in] байт, закодированный при помощи кода Хэмминга
    \return Декодированное число от 1 до 8
   */
  byte Hamming74 (uint8_t input);

  IRrecv *irrecv;
  decode_results results;

  /// счётчик принятых порций сообщения по 4 байта
  uint8_t rcv_counter = 0; // 1 - получены первые 4 байта; 2 - вторые 4 байта

  /// Принятые байты сообщения.
  /// \details Сообщение может быть неполным или содержать шумы.
  uint8_t received_data[MESSAGE_SIZE]; 
};
