#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#define led 12
#include <printf.h>
RF24 radio(7, 8); // CNS, CE
const uint64_t pipes[3] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0E2LL, 0xF0F0F0F0E3LL };

int ledPin =  9;
int ledState = LOW;
unsigned long SendpreviousMillis = 0;
unsigned long BuzpreviousMillis = 0;
const long BuzzerInterval = 300;
const long SendInterval = 1000;
unsigned long currentMillis =0;
int SensorGas[3];

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  printf_begin();
  pinMode(12, OUTPUT);
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipes[0]);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
  radio.printDetails();
  
}

void Monitoring(int sensorGas[]) {
  if (currentMillis - BuzpreviousMillis >= BuzzerInterval) {
    BuzpreviousMillis = currentMillis;
    if (sensorGas[1] > 50|| sensorGas[2] > 50 ) {
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
      digitalWrite(ledPin, ledState);
    }else{
      digitalWrite(ledPin, LOW);
      } 
    
  }
}

void SendSensorsData() {
  if (currentMillis - SendpreviousMillis >= SendInterval) {
    SendpreviousMillis = currentMillis;
    radio.write(&SensorGas, sizeof(SensorGas));
    Serial.print("Kirim Sensor Gas 1 : ");
    Serial.println(SensorGas[1]);
    Serial.print("Kirin Sensor Gas 2 : ");
    Serial.println(SensorGas[2]);
    Serial.println();
  }
}


void loop() {
  currentMillis = millis();
  delay(5);
  radio.stopListening();
  SensorGas[0] = 111;
  SensorGas[1] = analogRead(A0);
  SensorGas[2] = analogRead(A1);
  SendSensorsData();
  Monitoring(SensorGas);
}
