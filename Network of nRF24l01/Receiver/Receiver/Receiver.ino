#include <SPI.h>       // библиотека для работы с шиной SPI
#include "nRF24L01.h"  // nRF24l01
#include "RF24.h"      // nRF24l01

#define GREEN 3
#define RED 4

RF24 radio(9, 10);  // "создать" модуль на пинах 9 и 10 Для Нано
uint8_t pipe;

byte count, count1, count2;

double data1[] = { 0., 0., 0., 0. };
double data2[] = { 0., 0., 0. };
double trash[] = { 0., 0., 0., 0. };

// {BMP180: Temp, Pressure, Alt; Photo_res; DS18B20: Temp; DH21: Temp, Humidity}

void setup(void) {
  Serial.begin(19200);

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);

  radio.begin();                             // Инициализация модуля NRF24L01
  radio.setChannel(0x73);                    // Обмен данными будет вестись на пятом канале (2,405 ГГц)
  radio.setPALevel(RF24_PA_MIN);             // уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate(RF24_250KBPS);           // скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
                                             // должна быть одинакова на приёмнике и передатчике!
  radio.openReadingPipe(1, 0xAABBCCDD11LL);  // Открываем 1 трубу с идентификатором 1 передатчика 0xAABBCCDD11, для приема данных
  radio.openReadingPipe(2, 0xAABBCCDD22LL);  // Открываем 2 трубу с идентификатором 2 передатчика 0xAABBCCDD22, для приема данных

  radio.powerUp();         // начать работу
  radio.startListening();  // Включаем приемник, начинаем прослушивать открытые трубы
}

void loop() {
  if(radio.available(&pipe)){
    if (pipe == 1) {
      radio.read(&data1, sizeof(data1));
      digitalWrite(GREEN, HIGH);

      while (pipe != 2) {
        radio.available(&pipe);
        count1++;
        if (pipe == 2) {
          radio.read(&data2, sizeof(data2));
          break;

        } else {
          radio.read(&trash, sizeof(trash));
        }
      }

      makeString(data1, data2);

      pipe = 0;
      count = 0;
      digitalWrite(GREEN, LOW);
    }
    if (pipe == 2) {
      radio.read(&data2, sizeof(data2));
      digitalWrite(GREEN, HIGH);

      while (pipe != 1) {
        radio.available(&pipe);
        count2++;
        if (pipe == 1) {
          radio.read(&data1, sizeof(data1));
          break;

        } else {
          radio.read(&trash, sizeof(trash));
        }
      }

      makeString(data1, data2);

      pipe = 0;
      count = 0;
      digitalWrite(GREEN, LOW);
    }
  }
  else{
    count++;
    delay(100);
  }
  while(!radio.available(&pipe) && count>50){
    digitalWrite(RED, HIGH);
    delay(500);
    digitalWrite(RED, LOW);
    delay(500);
  }
}

void makeString(double *gotData1, double *gotData2){
  double data[] = { gotData1[0], gotData1[1], gotData1[2], gotData1[3], gotData2[0], gotData2[1], gotData2[2] };
  Serial.print(data[0]);
  Serial.print(", ");
  Serial.print(data[1]);
  Serial.print(", ");
  Serial.print(data[2]);
  Serial.print(", ");
  Serial.print(data[3]);
  Serial.print(", ");
  Serial.print(data[4]);
  Serial.print(", ");
  Serial.print(data[5]);
  Serial.print(", ");
  Serial.println(data[6]);
}
