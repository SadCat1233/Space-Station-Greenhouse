#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <LiquidCrystal.h>

LiquidCrystal lcd(6, 7, 8, 9, 10, 11);

int a = 7;
int b = 11;

RF24 radio(5, 13);  // "создать" модуль на пинах 9 и 10 Для Уно
bool flag = true;

String message = "";

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);         // открываем порт для связи с ПК
  radio.begin();              // активировать модуль
  radio.setPayloadSize(32);   // размер пакета, в байтах

  radio.openReadingPipe(1, address[0]);   // хотим слушать трубу 0
  radio.setChannel(0x60);     // выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX);   // уровень мощности передатчика
  radio.setDataRate (RF24_250KBPS); // скорость обмена

  radio.powerUp();        // начать работу
  radio.startListening(); // начинаем слушать эфир, мы приёмный модуль
}

void loop() {
  byte pipeNo;
  char gotByte;
  while (radio.available(&pipeNo)) {        // слушаем эфир со всех труб
    radio.read(&gotByte, sizeof(gotByte));  // чиатем входящий сигнал
    Serial.print(gotByte);
    int symbol = int(gotByte) - b;
    while (symbol % a != 0) {
      symbol = symbol + 128;
    }
    symbol = symbol / a;
    gotByte = char(symbol);
    if (gotByte == '\n') {
      Serial.println("");
      message = "";
      delay(1000);
      lcd.clear();
      break;
    }
    message += gotByte;
    if ((message.length() - 1) / 16 == 1){
      lcd.setCursor(message.length() % 17, (message.length() - 1) / 16);
    }
    else {
      lcd.setCursor(message.length() % 17 - 1, (message.length() - 1) / 16);
    }
    lcd.print(gotByte);
  }
}