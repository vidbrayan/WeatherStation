//========================================= 
//Code for Base weather station. 
//Responsible for receive data from RF and transmit via SPI to esp32 module with internet connecction 
//It's implemented on Arduino MEGA board 
//========================================= 
/*Module RX// Arduino MEGA  
    GND    ->   GND 

    Vcc    ->   3.3V 
    CE     ->   D9 
    CSN    ->   D7 
    CLK    ->   D52 
    MOSI   ->   D51 
    MISO   ->   D50  */ 
 
#include <SPI.h>   
#include <RF24.h>          
//-----RF Pines-------//  
#define CE_PIN 8 
#define CSN_PIN 9 
//========================================= 
//-----RF Configuration-------// 
RF24 radio(CE_PIN, CSN_PIN); // CE, CSN 
const byte address[6] = "00001"; 
//========================================= 
//-----Struct for data receive----// Air, Humidity, Temperature, Sound, Rain, WindSpeed   
struct Data_Package { 
  int air = 0; 
  float hum = 0.0; 
  float rain = 0.0; 
  float temp = 0.0; 
  float wind = 0.0; 
}; 
Data_Package data; 
//======================================== 
//---------Struct to send data trough SPI--------// 
struct TxDataSPI{ 
  char airQ[5]; 
  char humidity[5]; 
  char rain_m[5]; 
  char temperature[5]; 
  char wind_m[6]; 
}; 
TxDataSPI txData; 
//--------- markers and timing variables for sending frequency... 
  bool newTxData = false;  
 
  const byte startMarker = 255; 
  const byte txDataLen = sizeof(txData); 
 
  unsigned long prevUpdateTime = 0; 
  unsigned long updateInterval = 500; 
//======================================= 
void setup() { 
  Serial.begin(115200); 
  Serial3.begin(9600); 
  setupRF(); 
} 
//======================================= 
void loop() { 
  //Receive data from RF 
  rxData(); 
  debugDataRX(); 
  //Transfer data via SPI 
  updateDataToSend(); 
  spiDataTX(); 
  debugDataSPI(); 
  delay(200); 
} 
//======================================= 
//---------------------------------------------------------------------------------// 
//  SPI COMUNNICATION                                                              // 
//---------------------------------------------------------------------------------// 
void updateDataToSend() { 
 
    if (millis() - prevUpdateTime >= updateInterval) { 
      prevUpdateTime = millis(); 
      if (newTxData == false) { // ensure previous message has been sent           
        itoa(data.air, txData.airQ, 10); 
        dtostrf(data.hum, 2, 1, txData.humidity); 
        dtostrf(data.rain, 2, 1, txData.rain_m); 
        dtostrf(data.temp, 2, 1, txData.temperature); 
        dtostrf(data.wind, 2, 1, txData.wind_m); 
        newTxData = true; 
      } 
    } 
} 
void spiDataTX(){ 
  if(newTxData){ 
    Serial3.write(startMarker); 
    Serial3.write((byte*)&txData, txDataLen); 
    newTxData = false; 
  } 
} 
//---------------------------------------------------------------------------------// 
//  SETUP AND DATA RECEIVE MODULE RF                                               // 
//---------------------------------------------------------------------------------// 
void setupRF(){ 
  radio.begin(); 
  radio.openReadingPipe(0, address); 
  radio.setPALevel(RF24_PA_MIN); 
  radio.startListening(); 
} 
 
void rxData(){ 
  if(radio.available()){ 
    radio.read(&data, sizeof(Data_Package)); 
  } 
} 

 
