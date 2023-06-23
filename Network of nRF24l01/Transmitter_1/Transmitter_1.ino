#include <SPI.h>                                             // библиотека для работы с шиной SPI
#include "nRF24L01.h"                                        // nRF24l01
#include "RF24.h"                                            // nRF24l01

#include <Wire.h>                                            // BMP180
#include <Adafruit_BMP085.h>                                 // BMP180

#define DEBUG 0                                              // Отладка: 1, Рабочий режим: 0
#define SENDING_DELAY 1000                                   // Задержка отправки данных
#define PHOTO_SENSOR A0                                      // Объявление фоторезистора

RF24 radio(9, 10);                                           // "Создать" модуль на пинах 9 и 10 Для Нано
Adafruit_BMP085 bmp;                                         // BMP180

void setup(void){
  if(DEBUG){
    Serial.begin(9600);
    Serial.println("Режим отладки");
  }
  
  bmp.begin();                                               // Инициализация модуля BMP180

  radio.begin();                                             // Инициализация модуля NRF24L01
  radio.setChannel(0x73);                                    // Обмен данными будет вестись на пятом канале (2,405 ГГц)
  radio.setPALevel (RF24_PA_MAX);                            // Уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS);                          // Скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
                                                             // Должна быть одинакова на приёмнике и передатчике!
  radio.openWritingPipe (0xAABBCCDD11LL);                    // Открываем трубу с идентификатором 0xAABBCCDD11 для передачи данных (на ожном канале может быть открыто до 6 разных труб, которые должны отличаться только последним байтом идентификатора)

  radio.powerUp();                                           // Начать работу
  radio.stopListening();                                     // Не слушаем радиоэфир, мы передатчик
}

void loop(void){
  // Photo sensor
  double Photo_sensor = analogRead(PHOTO_SENSOR);
  

  // BMP180
  double Temp = bmp.readTemperature();
  double Pressure = bmp.readPressure();
  double Altitude = bmp.readAltitude();

  // Отладка
  if(DEBUG){
    printData(Photo_sensor, Temp, Pressure, Altitude);
  }

  double data[] = {Temp, Pressure, Altitude, Photo_sensor};
  radio.write(&data, sizeof(data));

  delay(SENDING_DELAY);
}

void printData(double Photo_sensor, double Temp, double Pressure, double Altitude){
  Serial.print("Photo sensor value: ");
  Serial.println(Photo_sensor);
  Serial.print("Temp = ");
  Serial.print(Temp);
  Serial.print(" C, Pressure = ");
  Serial.print(Pressure / 1000);
  Serial.print(" hPa, Alt = ");
  Serial.print(Altitude);
  Serial.println(" meters\n");
}

