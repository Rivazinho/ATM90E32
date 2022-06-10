#define ESP32

//Librerías
#include <ArduinoJson.h> //Librería formato texto
#include <WiFi.h> //Librería WiFi
#include <WiFiClient.h> //Librería cliente WiFi
#include <PubSubClient.h>  //Librería MQTT
#include <SPI.h> //Librería SPI
#include <ATM90E32.h> //Librería del MCU de la placa

//Variables auxiliares para el muestreo
unsigned long tiempo;
float Ts = 0.02;

//Calibración
unsigned short lineFreq = 389; //Frecuencia de red 50 Hz              
unsigned short PGAGain = 21; //100 A (2x)      
unsigned short VoltageGain = 7908; //Determinada experimentalmente
unsigned short CurrentGainCT1 = 39473; //Sonda SCT-016 120A/40mA
unsigned short CurrentGainCT2 = 39473; //Sonda SCT-016 120A/40mA
const int CS_pin = 5; //Chip Select para SPI

//Inicializar la clase IC
ATM90E32 eic{};

void setup() {
  delay(1000);
  //Inicializar el monitor serie
  Serial.begin(115200);
  eic.begin(CS_pin, lineFreq, PGAGain, VoltageGain, CurrentGainCT1, 0, CurrentGainCT2);
  delay(1000);
  
} 

void loop() {

  float voltageA, voltageC, totalVoltage, currentCT1, currentCT2, totalCurrent, activePower;

  voltageA = eic.GetLineVoltageA();
  voltageC = eic.GetLineVoltageC(); //Split single phase de los americanos, no vale para nada
  totalVoltage = voltageA;  
     
  currentCT1 = eic.GetLineCurrentA();
  currentCT2 = eic.GetLineCurrentC();
  totalCurrent = currentCT1 + currentCT2;
  
  activePower = eic.GetTotalActivePower();
  
  Serial.println(String(totalVoltage)+";"+String(totalCurrent)+";"+String(activePower));

}
