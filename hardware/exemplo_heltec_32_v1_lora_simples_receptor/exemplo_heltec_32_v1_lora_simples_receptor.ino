#include "heltec.h"

#define FREQUENCIA_LORA 915E6

#define MAX_ID_LENGHT 20
#define MAX_DEVICES 10

void handleReceivePacket(int);
void LoRaDataPrint();
void SerialDataPrint();
int findIndexById(const char*);
void parseData(String);

String packSize = "--";
String packet;

struct SensorData {
  char id[MAX_ID_LENGHT];
  float temperatura;
  float humidade;
  float luminosidade;
  int pacotesEnviados;
}

SensorData dataStore[MAX_DEVICES];
int currentIndex = 0;

void setup() {
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
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if(packetSize){
    handleReceivePacket(packetSize);
  }

}

void handleReceivePacket(int packetSize){
  packet = "";
  packSize = String(packetSize, DEC);
  for(int i=0; i < packetSize; i++){
    packet += (char) LoRa.read();
  }
  LoRaDataPrint();
  SerialDataPrint();
}

void LoRaDataPrint(){
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 1, "Recebendo "+ packSize +" bytes");
//  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(0, 16, "Dado:");
  Heltec.display->drawString(0, 38, packet);
  Heltec.display->display();
}

void SerialDataPrint(){
  Serial.println("Recebendo"+ packSize +" bytes");
  Serial.println("Dado:" + packet);
}

// Função para encontrar o índice de um ID_ESP no dataStore, ou -1 se não existir
int findIndexById(const char* id) {
    for (int i = 0; i < currentIndex; i++) {
        if (strcmp(dataStore[i].id, id) == 0) {
            return i; // Retorna o índice onde o ID foi encontrado
        }
    }
    return -1; // Retorna -1 se o ID não for encontrado
}

// Função para dividir a string em substrings e preencher a struct
void parseData(String input) {
    int startIdx = 0;
    int separatorIdx;
    int field = 0;

    char tempId[MAX_ID_LENGTH];
    float temperatura = 0;
    float humidade = 0;
    float luminosidade = 0;
    int pacotesEnviados = 0;

    // Separação dos dados usando indexOf e substring
    while (field < 5) {
        separatorIdx = input.indexOf(',', startIdx);
        String token = (separatorIdx != -1) ? input.substring(startIdx, separatorIdx) : input.substring(startIdx);

        switch (field) {
            case 0:
                token.toCharArray(tempId, MAX_ID_LENGTH);
                break;
            case 1:
                temperatura = token.toFloat();
                break;
            case 2:
                humidade = token.toFloat();
                break;
            case 3:
                luminosidade = token.toFloat();
                break;
            case 4:
                pacotesEnviados = token.toInt();
                break;
        }

        startIdx = separatorIdx + 1;
        field++;

        if (separatorIdx == -1) break; // Sai do loop se não houver mais delimitadores
    }

    if (field == 5) { // Verifica se todos os dados foram recebidos
        // Verifica se o ID já existe no dataStore
        int index = findIndexById(tempId);
        if (index != -1) {
            // Atualiza os dados do ID encontrado
            dataStore[index].temperatura = temperatura;
            dataStore[index].humidade = humidade;
            dataStore[index].luminosidade = luminosidade;
            dataStore[index].pacotesEnviados = pacotesEnviados;
            Serial.print("Dados atualizados para ID: ");
            Serial.println(tempId);
        } else {
            // Se o ID não existe, cria uma nova entrada
            if (currentIndex < MAX_ENTRIES) {
                strncpy(dataStore[currentIndex].id, tempId, MAX_ID_LENGTH);
                dataStore[currentIndex].temperatura = temperatura;
                dataStore[currentIndex].humidade = humidade;
                dataStore[currentIndex].luminosidade = luminosidade;
                dataStore[currentIndex].pacotesEnviados = pacotesEnviados;
                currentIndex++;
                Serial.print("Novo dado adicionado para ID: ");
                Serial.println(tempId);
            } else {
                Serial.println("Erro: Memória cheia, não é possível adicionar novos dados.");
            }
        }
    } else {
        Serial.println("Erro: Formato de string inválido.");
    }
}
