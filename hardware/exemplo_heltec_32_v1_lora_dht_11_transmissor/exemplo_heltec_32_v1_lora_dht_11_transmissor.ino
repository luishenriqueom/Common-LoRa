#include "heltec.h"
#include "DHTesp.h"

#define FREQUENCIA_LORA 915E6
#define DHT_PIN 17

DHTesp dht;

uint32_t espChipId = 0;
String packet;
float currentTemp;
float currentHumidity;
float currentLuminosity;
int countPacketsSend = 0;

void getTemp();
void sendPacket();
void getEspChipId();

void setup() {
  getEspChipId();
  Serial.begin(115200);

  Serial.println("Configurando Heltec...");  
  Heltec.begin(true, true, true, true, FREQUENCIA_LORA);
  Serial.println("Heltec configurado!");
  
  Serial.println("Iniciando display...");
  Heltec.display->init();
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->clear();
  Heltec.display->drawString(33, 5, "Iniciado");
  Heltec.display->drawString(10, 30, "com Sucesso");
  Heltec.display->display();
  delay(1000);

  dht.setup(DHT_PIN, DHTesp::DHT11);

  randomSeed(analogRead(0));

//  currentTemp = dht.getTemperature();
  
}

void loop() {
  getTemp();
//  currentTemp += 0.01;
//  currentHumidity += 0.05;  

  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);
  
  Heltec.display->drawString(30, 5, "Enviando");
  Heltec.display->drawString(33, 30, (String)currentTemp);
  Heltec.display->drawString(78, 30, "°C");
  Heltec.display->display();
  sendPacket();
  
//  if(currentTemp==40){
//    currentTemp=0;
//  }
//
//  if(currentHumidity==40){
//    currentHumidity=0;
//  }

  delay(1500);
}

void getTemp(){
  float temperature = dht.getTemperature();
  float humidity = dht.getHumidity();

  if(temperature != currentTemp){
    currentTemp = temperature;
  }

  if(humidity != currentHumidity){
    currentHumidity = humidity;
  }

  currentLuminosity = random(0, 100);
  
  delay(1000);
}

void sendPacket(){
  LoRa.beginPacket();
  LoRa.print(espChipId);
  LoRa.print(",");
  LoRa.print(currentTemp);
  LoRa.print(",");
  LoRa.print(currentHumidity);
  LoRa.print(",");
  LoRa.print(currentLuminosity);
  LoRa.print(",");
  LoRa.print(++countPacketsSend);
  LoRa.endPacket();
}

void getEspChipId(){
  for( int i=0; i < 17; i+=8){
     espChipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
}
