#include <SPI.h>       // библиотека для работы с шиной SPI
#include "nRF24L01.h"  // nRF24l01
#include "RF24.h"      // nRF24l01

#define GREEN 3
#define YELLOW 4
#define RED 5

RF24 radio(9, 10);  // "создать" модуль на пинах 9 и 10 Для Нано
uint8_t pipe;

byte count, count1, count2;   // Флаги состояний
bool firstTr = false;         
bool secondTr = false;

double data1[] = { 0., 0., 0., 0. };
double data2[] = { 0., 0., 0. };
double trash[] = { 0., 0., 0., 0. };

// {BMP180: Temp, Pressure, Alt; Photo_res; DS18B20: Temp; DH21: Temp, Humidity}

void setup() {
  Serial.begin(9600);

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);

  radio.begin();                             // Инициализация модуля NRF24L01

  radio.setChannel(0x73);                      // Обмен данными будет вестись на пятом канале (2,405 ГГц)
  radio.setPALevel(RF24_PA_MAX);             // уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate(RF24_250KBPS);           // скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
                                             // должна быть одинакова на приёмнике и передатчике!
  radio.openReadingPipe(1, 0xAABBCCDD11LL);  // Открываем 1 трубу с идентификатором 1 передатчика 0xAABBCCDD11, для приема данных
  radio.openReadingPipe(2, 0xAABBCCDD22LL);  // Открываем 2 трубу с идентификатором 2 передатчика 0xAABBCCDD22, для приема данных

  radio.powerUp();                           // начать работу
  radio.startListening();                    // Включаем приемник, начинаем прослушивать открытые трубы

  RYG_Indicator(100);
}

void loop() {
  if (radio.available(&pipe) && !firstTr && !secondTr) {  // Проверяем доступность обоих передатчиков в сети
    if (pipe == 1) {
      radio.read(&data1, sizeof(data1));

      while (pipe != 2) {
        radio.available(&pipe);
        
        if (pipe == 2) {
          radio.read(&data2, sizeof(data2));
          makeString(data1, data2);

          greenIndicator(50);

          count1 = 0;
          break;
        }

        else {
          radio.read(&trash, sizeof(trash));  // Уничтожаем ненужные данные
        }

        if (count1 > 100) {  // Ждем ответа передатчика, если нет ответа - работаем с одним из них
          firstTr = true;
          count1 = 0;
          break;
        }

        count1++;
        delay(100);
      }

      pipe = 0;
      count = 0;
    } 
    
    else if (pipe == 2) {
      radio.read(&data2, sizeof(data2));

      while (pipe != 1) {
        radio.available(&pipe);
        
        if (pipe == 1) {
          radio.read(&data1, sizeof(data1));
          makeString(data1, data2);

          greenIndicator(50);

          count2 = 0;
          break;
        }

        else {
          radio.read(&trash, sizeof(trash));
        }

        if (count2 > 100) {
          secondTr = true;
          count2 = 0;
          break;
        }

        count2++;
        delay(100);
      }

      pipe = 0;
      count = 0;
    }
  } 
  
  else if (radio.available(&pipe) && firstTr && !secondTr) {  // Работаем с одним из передатчиков
    if (pipe==1){
      radio.read(&data1, sizeof(data1));
      double data2[] = { 0., 0., 0. };
      makeString(data1, data2);

      yellowIndicator(50);
    }

    else {                    // Одновременно проверяем доступность второго передатчика, если второй в сети - работаем вместе с двумя
      firstTr = false;
      secondTr = false;
    }
    
    count = 0;
  } 
  
  else if (radio.available(&pipe) && secondTr && !firstTr) {
    if (pipe==2){
      radio.read(&data2, sizeof(data2));
      double data1[] = { 0., 0., 0., 0. };
      makeString(data1, data2);

      yellowIndicator(50);
    }
    
    else {
      firstTr = false;
      secondTr = false;
    }
    
    count = 0;
  } 
  
  else {            // Если оба передатчика не ответят, переходим в режим ожидания
    count++;
    delay(100);
  }

  while (!radio.available(&pipe) && count > 50) {   // Режим ожидания, постоянный опрос на наличие хотя бы одного передатчика в сети
    double data1[] = { 0., 0., 0., 0. };
    double data2[] = { 0., 0., 0. };
    makeString(data1, data2);

    redIndicator(500);

    firstTr = false;
    secondTr = false;
  }
}

// Функци индикации светодиодом
void greenIndicator(int time) {      
  digitalWrite(GREEN, HIGH);
  delay(time);
  digitalWrite(GREEN, LOW);
  delay(time);
}

void redIndicator(int time) {      
  digitalWrite(RED, HIGH);
  delay(time);
  digitalWrite(RED, LOW);
  delay(time);
}

void yellowIndicator(int time) {      
  digitalWrite(YELLOW, HIGH);
  delay(time);
  digitalWrite(YELLOW, LOW);
  delay(time);
}

void RYG_Indicator(int time){
  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, HIGH);
  digitalWrite(YELLOW, HIGH);
  delay(time);
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, LOW);
  delay(time);
  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, HIGH);
  digitalWrite(YELLOW, HIGH);
  delay(time);
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, LOW);
}

// Функция выдачи в порт полученных данных
void makeString(double *gotData1, double *gotData2) {        
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
