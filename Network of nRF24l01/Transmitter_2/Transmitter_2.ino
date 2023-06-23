#include <SPI.h>                                             // библиотека для работы с шиной SPI
#include "nRF24L01.h"                                        // nRF24l01
#include "RF24.h"                                            // nRF24l01

#include <OneWire.h>                                         // Библиотека для DS18B20
#include <DallasTemperature.h>                               // Библиотека для DS18B20

#include <DHT.h>                                             // Библиотека для DHT21

#define DEBUG 0                                              // Отладка: 1, Рабочий режим: 0
#define SENDING_DELAY 1000                                   // Задержка отправки данных
#define TEMP 4                                               // DS18B20
#define DHTPIN 3                                             // Пин, к которому подключен DHT21
#define DHTTYPE DHT21                                        // Тип датчика, здесь DHT21

RF24 radio(9, 10);                                           // "создать" модуль на пинах 9 и 10 Для Нано
byte counter;

OneWire oneWire(TEMP);                                       // Инициализация DS18B20
DallasTemperature sensor(&oneWire);                          

DHT dht(DHTPIN, DHTTYPE);                                    // Инициализация DHT21

void setup(void){

  if(DEBUG){
    Serial.begin(9600);
    Serial.println("Режим отладки");
  }

  sensor.begin();                                            // Датчик температуры DS18B20
  dht.begin();                                               // Датчик температуры и влажности DHT21

  radio.begin();                                             // Инициализация модуля NRF24L01
  radio.setChannel(0x73);                                      // Обмен данными будет вестись на пятом канале (2,405 ГГц)
  radio.setPALevel (RF24_PA_MAX);                            // Уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS);                          // Скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
                                                             // Должна быть одинакова на приёмнике и передатчике!
  radio.openWritingPipe (0xAABBCCDD22LL);                    // Открываем трубу с идентификатором 0xAABBCCDD11 для передачи данных (на ожном канале может быть открыто до 6 разных труб, которые должны отличаться только последним байтом идентификатора)

  radio.powerUp();                                           // Начать работу
  radio.stopListening();                                     // Не слушаем радиоэфир, мы передатчик
}

void loop(void){
  // DS18B20
  sensor.requestTemperatures();                               // Запрос текущей температуры
  double Temp1 = sensor.getTempCByIndex(0);                   // Получение значения температуры в градусах Цельсия

  // DH21
  double Temp2 = dht.readTemperature();                       // Считываем температуру
  double Humidity = dht.readHumidity();                       // Считываем влажность

  // Отладка
  if(DEBUG){
    printData(Temp1, Temp2, Humidity);
  }

  // Отправка
  double data[] = {Temp1, Temp2, Humidity};
  radio.write(&data, sizeof(data));

  delay(SENDING_DELAY);
}

void printData(double Temp1, double Temp2, double Humidity){
  Serial.print("Temperature (DS18B20): ");
  Serial.print(Temp1);
  Serial.print(" C, ");
  Serial.print("Temperature (DH21): ");
  Serial.print(Temp2);
  Serial.print(" C, Humidity (DH21): ");
  Serial.print(Humidity);
  Serial.println(" %");
}

