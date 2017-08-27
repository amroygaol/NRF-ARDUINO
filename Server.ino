#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>


RF24 radio(7, 8); // CNS, CE
const uint64_t pipes[3] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0E2LL, 0xF0F0F0F0E3LL };
LiquidCrystal_I2C  lcd(0x3F, 2, 1, 0, 4, 5, 6, 7);
// Addr, En, Rw, Rs, d4, d5, d6, d7, backlighpin, polarity

int ledPin =  9;
unsigned long LcdpreviousMillis = 0;
unsigned long BuzpreviousMillis = 0;
const long Buzinterval = 300;
const long Lcdinterval = 500;
unsigned long currentMillis = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  printf_begin();
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(0, pipes[0]);
  radio.openReadingPipe(1, pipes[1]);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
  radio.printDetails();
  lcd.begin(20, 4);
  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.clear();
}

class ServerNode
{
  public:
    int Buzer[4];
    int GasSensors[6];
    int sensor[4];
    bool PlaySoundNotif = false;
    int ledState = LOW;

    void SetSensorsData() {
      delay(5);
      if (radio.available()) {
        radio.read(&GasSensors, sizeof(GasSensors));
        if (GasSensors[0] == 111) {
          sensor[0] = GasSensors[1];
          sensor[1] = GasSensors[2];
        }
        if (GasSensors[3] == 222) {
          sensor[2] = GasSensors[4];
          sensor[3] = GasSensors[5];
          GetSensorsData();
        }
      }
    }

    void GetSensorsData() {
      for (int i = 0 ; i <= 3 ; i++) {
        Serial.print("Data Sensor Gas ");
        Serial.print(i + 1);
        Serial.print(" = ");
        Serial.println(sensor[i]);
      }
      Serial.println();
    }


    void GetDataInLcd() {
      if (currentMillis - LcdpreviousMillis >= Lcdinterval) {
        LcdpreviousMillis = currentMillis;
        lcd.clear();
        for (int i = 0 ; i <= 3 ; i++) {
          lcd.setCursor(0, i);
          lcd.print("Ruangan ");
          lcd.print(i + 1);
          lcd.print(" = ");
          lcd.setCursor(12, i);
          lcd.print(sensor[i]);
        }
      }
    }

    void Monitoring() {
      if (currentMillis - BuzpreviousMillis >= Buzinterval) {
        BuzpreviousMillis = currentMillis;
        if (sensor[0] >= 50 || sensor[1] >= 50  || sensor[2] >= 120  || sensor[3] >= 120 ) {
          PlaySoundNotif = true;
          if (ledState == LOW) {
            ledState = HIGH;
          } else {
            ledState = LOW;
          }
          digitalWrite(ledPin, ledState);
        } else {
          PlaySoundNotif = false;
          digitalWrite(ledPin, LOW);
        }
      }
    }

    void SoundNotif() {
      if (PlaySoundNotif = true) {
        //          statement
      } else {
        //          statement
      }
    }
};

ServerNode server;

void loop() {
  //  Serial.println(server.sensor1);
  currentMillis = millis();
  server.SetSensorsData();
  server.GetDataInLcd();
  server.Monitoring();
}

