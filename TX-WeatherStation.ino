//=========================================  

//Code for field weather station.  

//Responsible for take data from enviroment and transmit through RF module (TX)  

//It's mplement on Arduino NANO board  

//=========================================  

#include <SPI.h>  

#include <RF24.h>   

#include <DHT.h>  

//-----Sensor Pin-----//  

#define PIN_ANEM 3  

#define PIN_PLUV 2  

#define PIN_DHT 5  

#define PIN_MQ 6  

//----RF Pines-------//  

#define CE_PIN 9  

#define CSN_PIN 10  

//=========================================  

//-----RF Configuration-------//  

RF24 radio(CE_PIN, CSN_PIN); // CE, CSN  

const byte address[6] = "00001";  

//=========================================  

//-----Variables for Rain Gauge----//  

const int timeThreshold = 170;  

float amountRain = 0, counter = 0; // Cantidad de Agua por cada tick -> 1.818  

unsigned long timePluv = 0;  

int tick = 0;  

//=========================================  

//-----Variables for Wind Speed----//  

volatile int encoderPulse = 0;  

unsigned long timeWind = 0;  

byte resolution = 20;  

const float wheelDiameter = 0.24; //24cm, diametro de la aspa  

unsigned int RPM = 0;  

float windSpeed = 0.0, w = 0.0;  

//=========================================  

//------Variables for DHT Humidity/Temperature  

float temperature = 0.0, humidity = 0.0;  

DHT dht(PIN_DHT, DHT11);  

//=========================================  

//------Variables for Air Quality-----//  

int mqRead = 0;  

//=========================================  

//-----Struct for data trasfer----// Air, Humidity, Temperature, Sound, Rain, WindSpeed    

struct Data_Package{  

  int air = 0;  

  float hum = 0.0;  

  float rain = 0.0;  

  float temp = 0.0;  

  float wind = 0.0;  

};  

Data_Package data;  

//Setup  

void setup() {  

  Serial.begin(115200);  

  PinSetup();  

  setupRF();  

  dht.begin();  

}  

  

void loop() {  

  WindSpeed();  

  dhtMeasurement();  

  airMeasurement();  

  txData();  

  //-----Debug Data-------//  

  Serial.print("Velocidad Viento: "); Serial.print(data.wind); Serial.println(" Km/h");  

  Serial.print("temperatura: "); Serial.print(data.temp); Serial.print(" Humedad: "); Serial.print(data.hum);  

  Serial.print(" Aire: "); Serial.print(data.air, DEC); Serial.println(" ppm");  

  if(counter != data.rain){  

    Serial.print("Rain: "); Serial.print(data.rain); Serial.println(" mm");  

    counter = data.rain;  

  }  

  delay(1000);  

}  

//---------------------------------------------------------------------------------//  

//  PINES SETUP                                                                    //  

//---------------------------------------------------------------------------------//  

void PinSetup(){  

  //Pins Define  

  pinMode(PIN_ANEM,INPUT_PULLUP);  

  pinMode(PIN_PLUV,INPUT_PULLUP);  

  //Attach Interrupts  

  attachInterrupt(digitalPinToInterrupt(PIN_ANEM), InterruptANEM, FALLING);  

  attachInterrupt(digitalPinToInterrupt(PIN_PLUV), InterruptPLUV, FALLING);  

  //Other oines  

  pinMode(PIN_DHT, INPUT);  

  pinMode(PIN_MQ, INPUT);  

}  

//---------------------------------------------------------------------------------//  

// WIND SPEED CALCULATE                                                            //  

//---------------------------------------------------------------------------------//  

void WindSpeed(){  

  if(millis() - timeWind >= 1000){  

    detachInterrupt(PIN_ANEM);  

    RPM = (encoderPulse * (60/resolution));  

    w=((RPM*6.2832)/60);  

    windSpeed = w * wheelDiameter * 3.6; //1m/s = 3.6Km/h  

    data.wind = windSpeed;  

    encoderPulse = 0;  

    timeWind = millis();  

    attachInterrupt(digitalPinToInterrupt(PIN_ANEM), InterruptANEM, FALLING);  

  }  

}  

//---------------------------------------------------------------------------------//  

// DHT HUMIDITY/TEMPERATURE                                                        //  

//---------------------------------------------------------------------------------//  

void dhtMeasurement(){  

  temperature = dht.readTemperature(); data.temp = temperature;  

  humidity = dht.readHumidity(); data.hum = humidity;  

}  

//---------------------------------------------------------------------------------//  

// Air Polution / Air Quality                                                      //  

//---------------------------------------------------------------------------------//  

void airMeasurement(){  

  mqRead = analogRead(PIN_MQ); data.air = mqRead;  

}  

//---------------------------------------------------------------------------------//  

// INTERRUPTS                                                                      //  

//---------------------------------------------------------------------------------//  

void InterruptANEM(){  

  encoderPulse++;  

}  

void InterruptPLUV(){  

  if (millis() - timePluv > timeThreshold)  

{  

    tick++;  

    amountRain = tick * 1.818; data.rain = amountRain;  

timePluv = millis();  

}  

}  

  

//---------------------------------------------------------------------------------//  

// SETUP AND DATA TRANSFER MODULE RF                                               //  

//---------------------------------------------------------------------------------//  

void setupRF(){  

  radio.begin();  

  radio.openWritingPipe(address);  

  radio.setPALevel(RF24_PA_MIN);  

  radio.stopListening();  

}  

  

void txData(){  

  bool report = radio.write(&data, sizeof(Data_Package));  

  if(report){  

    Serial.println(F("Transmission successful! "));  

  } else{  

    Serial.println(F("Transmission Failed! or time out"));  

  }  

} 

 
