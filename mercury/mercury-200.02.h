// Telegram @RocketFox

#include "esphome.h"

class Mercury : public PollingComponent, public UARTDevice {
  Sensor *Volts {nullptr}; // Сенсоры
  Sensor *Amps {nullptr};
  Sensor *Watts {nullptr};
  Sensor *Tariff1 {nullptr};
  Sensor *Tariff2 {nullptr};
  Sensor *Tariff3 {nullptr};
  Sensor *Sum_Tariff {nullptr};

  int seriall = 420095;  // сюда свой серийный номер счетчика

  public:
    Mercury(UARTComponent *parent, Sensor *sensor1, Sensor *sensor2, Sensor *sensor3, Sensor *sensor4, Sensor *sensor5, Sensor *sensor6, Sensor *sensor7) : UARTDevice(parent) , Volts(sensor1) , Amps(sensor2) , Watts(sensor3), Tariff1(sensor4), Tariff2(sensor5), Tariff3(sensor6), Sum_Tariff(sensor7) {}

  unsigned char electrical_parameters[7]; // Байты на получене мгновенных значений
  unsigned char tarif[7]; // Байты на получение тариффа

  byte Re_buf[100];
  int counter=0;
  double V, A, W;
  double T1, T2, T3, sum;
  

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

uint16_t crc16(const uint8_t *data, uint8_t len) {
  uint16_t crc = 0xFFFF;
  while (len--) {
    crc ^= *data++;
    for (uint8_t i = 0; i < 8; i++) {
      if ((crc & 0x01) != 0) {
        crc >>= 1;
        crc ^= 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }
  return crc;
}

void calculateParams(unsigned char* frame, uint32_t serial_, unsigned char comm) {
  frame[0] = 0x00;
  frame[1] = serial_ >> 16;
  frame[2] = serial_ >> 8;
  frame[3] = serial_;
  frame[4] = comm;
  auto crc = crc16(frame, 5);
  frame[5] = crc >> 0;
  frame[6] = crc >> 8;
  }


  void setup() override {
    this -> set_update_interval(120000); // Периуд обновления сенсора
    calculateParams(electrical_parameters, seriall, 0x63);
    calculateParams(tarif, seriall, 0x27);
  }

void loop() override {
}

void main_uart_read (byte *command) {
  write_array(command, 7);

  delay(30);

    while (available()) { // Читение и запись данных из UART
      Re_buf[counter]=read();
      counter++;
    }

  delay(100);

}

  void update() override {

    main_uart_read(tarif);

    main_uart_read(electrical_parameters);

    counter = 0;

  if(Re_buf[0] == 0x00 && Re_buf[27] == 0x63) {

    V = readDouble(&Re_buf[28], 10); // Парсинг байтов и перевод в нормальные значения
    A = readDouble(&Re_buf[30], 100); // Парсинг байтов и перевод в нормальные значения
    W = readDouble<3>(&Re_buf[32], 1000); // Парсинг байтов и перевод в нормальные значения

  }

  if ( Re_buf[0] == 0x00 && Re_buf[4] == 0x27 ) {

    T1 = readDouble<4>(&Re_buf[5], 100);
    T2 = readDouble<4>(&Re_buf[9], 100);
    T3 = readDouble<4>(&Re_buf[13], 100);
    sum = T1 + T2 + T3;

  }

    if (Volts != nullptr) Volts -> publish_state(V); // Отправка в как сенсор
    if (Amps != nullptr) Amps -> publish_state(A);
    if (Watts != nullptr) Watts -> publish_state(W);
    if (Tariff1 != nullptr) Tariff1 -> publish_state(T1);
    if (Tariff2 != nullptr) Tariff2 -> publish_state(T2);
    if (Tariff3 != nullptr) Tariff3 -> publish_state(T3);
    if (Sum_Tariff != nullptr) Sum_Tariff -> publish_state(sum);

  };

};