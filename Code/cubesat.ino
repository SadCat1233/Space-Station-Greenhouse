#include <TroykaThermometer.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "DHT.h"

RF24 radio(10, 9);
DHT dht(2, DHT22); 

int a = 7;
int b = 11;

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"};
char output_char;
char coded_output_char;

TroykaThermometer thermometer(A0);
int inputPin = A1;

void setup() {
  Serial.begin(9600);
  pinMode(inputPin, INPUT);
  radio.begin();
  dht.begin();

  radio.setPayloadSize(32);

  radio.openWritingPipe(address[0]);
  radio.setChannel(0x60);

  radio.setPALevel (RF24_PA_MAX);
  radio.setDataRate (RF24_250KBPS);

  radio.powerUp();
  radio.stopListening();
}

void func(String message) {
  for (int i = 0; i < message.length(); i++) {
    output_char = char(message[i]);
    coded_output_char = char((int(output_char) * a + b) % 128);
    radio.write(&coded_output_char, sizeof(output_char));
    delay(40);
  }
  delay(1000);
}

void loop() {
  thermometer.read();
  float humidity = dht.readHumidity();
  String light_output_message;
  if ((1024 - analogRead(A1)) < 400) {
    light_output_message = "Light: low light\n";
  }
  else if ((1024 - analogRead(A1)) > 600) {
    light_output_message = "Light: high light\n";
  }
  else {
    light_output_message = "Light: normal light\n";
  }

  func(light_output_message);
  Serial.println("Light: " + String(1024 - analogRead(A1)) + "\n");

  String humidity_status = humidity > 60.0 ? "OK\n" : "NOT OK\n";
  func("Humidity: " + humidity_status);
  Serial.println("Humidity: " + String(humidity) + "\n");

  func(String("Temperature: " + String(thermometer.getTemperatureC())) + " C\n");
  Serial.println(String("Temperature: " + String(thermometer.getTemperatureC())) + " C\n");
}