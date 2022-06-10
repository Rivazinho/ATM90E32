#include <ArduinoJson.h> //Librería formato texto
#include <SPI.h> //Librería SPI
#include <ATM90E32.h> //Librería del MCU de la placa

//Variables auxiliares para el muestreo
unsigned long tiempo;
float Ts = 0.05;
//Variables para envío de datos
char JSONmessageBuffer[255];
int SpiByte;

//Calibración
unsigned short lineFreq = 389; //Frecuencia de red 50 Hz              
unsigned short PGAGain = 21; //100 A (2x)      
unsigned short VoltageGain = 7908; //Determinada experimentalmente
unsigned short CurrentGainCT1 = 39473; //Sonda SCT-016 120A/40mA
unsigned short CurrentGainCT2 = 39473; //Sonda SCT-016 120A/40mA
const int CS_pin = 0; //Chip Select para SPI

//Inicializar la clase IC
ATM90E32 eic{};

void setup() {
  //Inicializar las lecturas del microcontrolador
  eic.begin(CS_pin, lineFreq, PGAGain, VoltageGain, CurrentGainCT1, 0, CurrentGainCT2);
  delay(1000);
  //Activar SPI en modo esclavo
  SPCR |= bit(SPE);
  //Envío en Slave Out
  pinMode(MISO, OUTPUT);
  //Activar interrupción SPI
  SPI.attachInterrupt();
}

void loop () { 
  //Buffer JSON 
  StaticJsonDocument<255> meterData;
 
  float tension, intensidad, potencia, factor;
  //Lecturas a 50 ms
  if ((millis()-tiempo)>(Ts)){
  
  tension = eic.GetLineVoltageA();
  intensidad = eic.GetLineCurrentA();
  potencia = eic.GetTotalActivePower();
  factor = eic.GetTotalPowerFactor();
  }

  //Buffer JSON
  meterData["V1"] = tension;
  meterData["I1"] = intensidad;
  meterData["AP"] = potencia;
  meterData["PF"] = factor;
  
  serializeJson(meterData,JSONmessageBuffer);
  
  tiempo = millis(); 
}

//Rutina de interrupción SPI
ISR (SPI_STC_vect)
{
  SpiByte = SPDR;
  SPDR = JSONmessageBuffer; //Envío de datos por SPI
}  
