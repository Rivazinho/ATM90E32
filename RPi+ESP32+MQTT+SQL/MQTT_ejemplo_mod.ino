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
float Ts = 0.05;

//Configuración WiFi
const char* ssid = "D142"; //SSID
const char* password =  "k1000ooo"; //Contraseña

//Configuración MQTT
const char* mqttServer = "192.168.0.107"; //Server
const int mqttPort = 1883; //Puerto
const char* mqttUser = "carlos"; //Usuario
const char* mqttPassword = "facendo40"; //Contraseña
const char* HATopic = "home/energy/sensor"; //Topic 

//Calibración
unsigned short lineFreq = 389; //Frecuencia de red 50 Hz              
unsigned short PGAGain = 21; //100 A (2x)      
unsigned short VoltageGain = 7908; //Determinada experimentalmente
unsigned short CurrentGainCT1 = 39473; //Sonda SCT-016 120A/40mA
unsigned short CurrentGainCT2 = 39473; //Sonda SCT-016 120A/40mA
const int CS_pin = 5; //Chip Select para SPI

//Inicializar la clase IC
ATM90E32 eic{}; 

//Creación del cliente MQTT
WiFiClient espClient;
PubSubClient client(espClient);

//Setup
void setup() {
  delay(2000);
  //Inicializar el monitor serie
  Serial.begin(115200);

  //Configuración WiFi
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Conectando a WiFi...");
  }
 
  Serial.println("Conectado a Wifi con éxito");

  //Configuración MQTT 
  client.setServer(mqttServer, mqttPort);
 
  while (!client.connected()) {
    Serial.println("Conectando a MQTT...");
 
    if (client.connect("EnergyMeterClient", mqttUser, mqttPassword )) {
 
      Serial.println("Conectado");
 
    } else {
 
      Serial.print("Fallo con estado ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 
  Serial.println("Iniciar ATM90E32");
  //Inicializar las lecturas del microcontrolador
  eic.begin(CS_pin, lineFreq, PGAGain, VoltageGain, CurrentGainCT1, 0, CurrentGainCT2);
  delay(1000);
  
} 

//Loop
 
void loop() {
  //Buffer JSON 
  StaticJsonDocument<256> meterData;
 
  float voltageA, voltageC, totalVoltage, currentCT1, currentCT2, totalCurrent, activePower, powerFactor, temp, freq, totalWatts;

  //Variables auxiliares para el estado de la placa
  unsigned short sys0 = eic.GetSysStatus0(); //EMMState0
  unsigned short sys1 = eic.GetSysStatus1(); //EMMState1
  unsigned short en0 = eic.GetMeterStatus0();//EMMIntState0
  unsigned short en1 = eic.GetMeterStatus1();//EMMIntState1

  Serial.println("Sys Status: S0:0x" + String(sys0, HEX) + " S1:0x" + String(sys1, HEX));
  Serial.println("Meter Status: E0:0x" + String(en0, HEX) + " E1:0x" + String(en1, HEX));
  delay(10);

  if (sys0 == 65535 || sys0 == 0) Serial.println("Error: No se reciben datos del energy meter - comprueba el conexionado");

//Lecturas a 50 ms
if ((millis()-tiempo)>(Ts)){
  
  //Lectura de voltajes
  voltageA = eic.GetLineVoltageA();
  voltageC = eic.GetLineVoltageC(); //Split single phase de los americanos, no vale para nada
  totalVoltage = voltageA;     

  //Lectura de intensidades
  currentCT1 = eic.GetLineCurrentA();
  currentCT2 = eic.GetLineCurrentC();
  totalCurrent = currentCT1 + currentCT2;

  //Lectura de potencia, factor de potencia, temperatura y frecuencia
  activePower = eic.GetTotalActivePower();
  powerFactor = eic.GetTotalPowerFactor();
  //temp = eic.GetTemperature();
  //freq = eic.GetFrequency();
  //totalWatts = (voltageA * currentCT1) + (voltageC * currentCT2);
}

/*Monitor serie para pruebas 
  Serial.println("Voltage 1: " + String(voltageA) + "V");
  Serial.println("Voltage 2: " + String(voltageC) + "V");
  Serial.println("Current 1: " + String(currentCT1) + "A");
  Serial.println("Current 2: " + String(currentCT2) + "A");
  Serial.println("Active Power: " + String(realPower) + "W");
  Serial.println("Power Factor: " + String(powerFactor));
  Serial.println("Fundimental Power: " + String(eic.GetTotalActiveFundPower()) + "W");
  Serial.println("Harmonic Power: " + String(eic.GetTotalActiveHarPower()) + "W");
  Serial.println("Reactive Power: " + String(eic.GetTotalReactivePower()) + "var");
  Serial.println("Apparent Power: " + String(eic.GetTotalApparentPower()) + "VA");
  Serial.println("Phase Angle A: " + String(eic.GetPhaseA()));
  Serial.println("Chip Temp: " + String(temp) + "C");
  Serial.println("Frequency: " + String(freq) + "Hz");
*/

//MQTT usando el buffer JSON
  meterData["V1"] = voltageA;
  //meterData["V2"] = voltageC;
  meterData["I1"] = currentCT1;
  //meterData["I2"] = currentCT2;
  //meterData["totI"] = totalCurrent;
  meterData["AP"] = activePower;
  meterData["PF"] = powerFactor;
  //meterData["t"] = temp;
  //meterData["f"] = freq;
  
  char JSONmessageBuffer[200];
  serializeJson(meterData,JSONmessageBuffer);
  Serial.println("Enviando mensaje al topic MQTT...");
  Serial.println(JSONmessageBuffer);

 //Publicación MQTT
  if (client.publish(HATopic, JSONmessageBuffer) == true) {
    Serial.println("Éxito enviando mensaje");
  } else {
    Serial.println("Error enviando mensaje");
  }
 
  client.loop();
  Serial.println("-------------");
  
  tiempo = millis();
 
}
