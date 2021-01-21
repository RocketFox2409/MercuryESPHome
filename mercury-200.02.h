// Telegram @RocketFox

#include "esphome.h"

class Mercury : public PollingComponent, public UARTDevice {
  Sensor *Volts {nullptr}; // Сенсоры
  Sensor *Amps {nullptr};
  Sensor *Watts {nullptr};
  Sensor *Tariff1 {nullptr};
  Sensor *Tariff2 {nullptr};
  Sensor *Sum_Tariff {nullptr};

  public:
    Mercury(UARTComponent *parent, Sensor *sensor1, Sensor *sensor2, Sensor *sensor3, Sensor *sensor4, Sensor *sensor5, Sensor *sensor6) : UARTDevice(parent) , Volts(sensor1) , Amps(sensor2) , Watts(sensor3), Tariff1(sensor4), Tariff2(sensor5), Sum_Tariff(sensor6) {}

/*
  Разбор запроса 
  Например
  0x00, 0x06, 0x68, 0xff, 0x63, 0xA4, 0x8D
  0x00 - Стартовый байт
  0x06, 0x68, 0xff - Адрес счетчика в HEX (3 байта) на меркурий 200.02 последние 6 цифр
  0x63 - Запрос (63 - мгновенные значения)
  0xA4, 0x8D - CRC16 (Modbus)  Рассчитовал на этом сайте https://www.lammertbies.nl/comm/info/crc-calculation
  Сумма байтов 0x00, 0x06, 0x68, 0xff, 0x63 ответ сдвигается (на сайте 0x8DA4 нужно 0xA4 0x8D)

  https://www.incotexcom.ru/files/em/docs/mercury-protocol-obmena-1.pdf - Протокол обмена однофазных счетчиков Меркурий 200, 201, 203 (кроме Меркурий 203.2TD), 206
*/

  byte electrical_parameters[7] = {  0x00, 0x06, 0x68, 0xff, 0x63, 0xA4, 0x8D  }; // Байты на получене мгновенных значений
  byte tarif[7] = {  0x00, 0x06, 0x68, 0xff, 0x27, 0xA4, 0xBE  }; // Байты на получение тариффа

  byte Re_buf[100];
  int counter=0;
  double V, A, W;
  double T1, T2, sum;

////////////////////// 
typedef unsigned char uchar;

long pow(long a, int s) {
  long out = 1;
  for (int i = 0; i < s; i++) out *= a;
  return out;
}

template <size_t N = 2>
long readLong(uchar *inp) {
  long out = 0;

  for (int i = 0; i < N; i++) {
    uchar v = inp[i];
    int p = pow(10, ((N - 1) - i) * 2);
    out += (((v >> 4) & 15) * 10 + (v & 15)) * p;
  }

  return out;
}

template <size_t N = 2>
double readDouble(uchar *inp, int del) {
  return (double)readLong<N>(inp) / del;
}
//////////////////////


  void setup() override {
    this -> set_update_interval(120000); // Периуд обновления сенсора 
  }

void loop() override {
}


  void update() override {

  write_array(tarif, sizeof(tarif));

  delay(30);

    while (available()) { // Читение и запись данных из UART
      Re_buf[counter]=read();
      // ESP_LOGD("main", "valve: %d", Re_buf[counter]);
      counter++;
    }

    delay(100);

    write_array(electrical_parameters, sizeof(electrical_parameters));

    delay(30);

    while (available()) { // Читение и запись данных из UART
      Re_buf[counter]=read();

      counter++;
    }

    counter = 0;

  if(Re_buf[0] == 0x00 && Re_buf[27] == 0x63) {

    V = readDouble(&Re_buf[28], 10); // Парсинг байтов и перевод в нормальные значения
    A = readDouble(&Re_buf[30], 100); // Парсинг байтов и перевод в нормальные значения
    W = readDouble<3>(&Re_buf[32], 1000); // Парсинг байтов и перевод в нормальные значения

  }

  if ( Re_buf[0] == 0x00 && Re_buf[4] == 0x27 ) {

    T1 = readDouble<4>(&Re_buf[5], 100);
    T2 = readDouble<4>(&Re_buf[9], 100);
    sum = T1 + T2;


  }

    if (Volts != nullptr) Volts -> publish_state(V); // Отправка в как сенсор
    if (Amps != nullptr) Amps -> publish_state(A);
    if (Watts != nullptr) Watts -> publish_state(W);
    if (Tariff1 != nullptr) Tariff1 -> publish_state(T1);
    if (Tariff2 != nullptr) Tariff2 -> publish_state(T2);
    if (Sum_Tariff != nullptr) Sum_Tariff -> publish_state(sum);

  };

};