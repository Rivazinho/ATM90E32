#define ESP32

//Librerías
#include <ATM90E32.h> //Librería del MCU de la placa

//Variables auxiliares para el muestreo
unsigned long tiempo;
float Ts = 0.05;

//Calibración
unsigned short lineFreq = 389; //Frecuencia de red 50 Hz              
unsigned short PGAGain = 21; //100 A (2x)      
unsigned short VoltageGain = 7908; //Determinada experimentalmente
unsigned short CurrentGainCT1 = 39473; //Sonda SCT-016 120A/40mA
unsigned short CurrentGainCT2 = 39473; //Sonda SCT-016 120A/40mA
const int CS_pin = 5; //Chip Select para SPI

//Inicializar la clase IC
ATM90E32 eic{}; 

//Setup
void setup() {
  delay(1000);
  
  //Inicializar el monitor serie
  Serial.begin(9600);
  
  //Inicializar las lecturas del microcontrolador
  Serial.println("Iniciar ATM90E32");
  eic.begin(CS_pin, lineFreq, PGAGain, VoltageGain, CurrentGainCT1, 0, CurrentGainCT2);
  delay(1000); 
} 

//Loop
void loop() {
 
  float voltage, current, activePower, powerFactor;

//Lecturas a 50 ms
if ((millis()-tiempo)>(Ts)){
  
  voltage = eic.GetLineVoltageA();  
  current = eic.GetLineCurrentA();
  activePower = eic.GetTotalActivePower();
  powerFactor = eic.GetTotalPowerFactor();
}

//Monitor serie para pruebas 
  Serial.println("Voltage: " + String(voltage) + "V");
  Serial.println("Current: " + String(current) + "A");
  Serial.println("Active Power: " + String(activePower) + "W");
  Serial.println("Power Factor: " + String(powerFactor));

  tiempo = millis();
 
}
