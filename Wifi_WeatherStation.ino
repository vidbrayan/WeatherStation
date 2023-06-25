//===================  

// Using Serial to send and receive structs between two Arduinos  

// the Tx is sending at 500msec intervals  

// it is assumed that this Rx program can loop in less than half that time  

//===================  

#include <SPI.h>  

#include <WiFi.h>  

#include <ThingSpeak.h>  

  

#define RXp2 16  

#define TXp2 17  

  

//=======================================  

//-----For WIFI Conection-----//  

  const char* ssid = "Alejos";  

  const char* wpa = "HJko7835-21Wsm";  

//-----For ThingSpeak--------//  

  unsigned long chanelID = 2133905;  

  const char* WriteAPIKey = "Q50E711QT4UKXXJR";  

  long refreshTime = 0;  

  const int timeUpload = 15000;  

  WiFiClient client;  

  bool isWiFiConnected = false;  

//=======================================  

struct RxStruct {  

  char airQ[5];  

  char humidity[5];  

  char rain_m[5];  

  char temperature[5];  

  char wind_m[6];  

};  

//---------Markers and control variables----------  

  RxStruct rxData;  

  bool newData = false;  

  const byte startMarker = 255;  

  const byte rxDataLen = sizeof(rxData);  

//=================================  

  

void setup() {  

  Serial.begin(115200);  

  Serial.println("\nStarting SerialStructReceive.ino\n");  

  // setup Serial for communication  

  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);  

  WIFI_Conection(); // WIfi SetUp  

  ThingSpeak.begin(client);  // Initialize ThingSpeak  

}  

  

//=======================================  

  

void loop() {  

  //-------Receive data from SPI--------------  

  recvStructWithKnownLength();  

  // this bit checks if a message has been received  

  if (newData) {  

      showNewData();  

      newData = false;  

  }  

  //------Load data to ThingSpeak-------------  

  if(isWiFiConnected){  

    UploadData();  

  }  

}  

//=======================================  

//---------------------------------------------------------------------------------//  

// RECEIVE DATA FROM SPI CONNECTION                                                //  

//---------------------------------------------------------------------------------//  

void recvStructWithKnownLength() {  

    static byte recvData[rxDataLen];  

    byte rb;  

    byte * structStart;  

    structStart = reinterpret_cast <byte *> (&rxData);  

  

    if (Serial2.available() >= rxDataLen + 1 and newData == false) {  

      rb = Serial2.read();  

  

      if (rb == startMarker) {  

          // copy the bytes to the struct  

        for (byte n = 0; n < rxDataLen; n++) {  

          *(structStart + n) = Serial2.read();  

        }  

            // make sure there is no garbage left in the buffer  

        while (Serial2.available() > 0) {  

          byte dumpTheData = Serial2.read();  

        }  

        newData = true;  

      }  

    }  

}  

  

//---------------------------------------------------------------------------------//  

// THINGSPEAK -> UPLOAD DATA                                                       //  

//---------------------------------------------------------------------------------//  

void UploadData(){  

  if(millis() - refreshTime  > timeUpload){  

    Serial.println("Upload Data");  

    ThingSpeak.setField(1, rxData.rain_m);  

    ThingSpeak.setField(2, rxData.wind_m);  

    ThingSpeak.setField(3, rxData.temperature);  

    ThingSpeak.setField(4, rxData.humidity);  

    ThingSpeak.setField(5, rxData.airQ);  

    ThingSpeak.writeFields(chanelID,WriteAPIKey);  

    refreshTime = millis();  

  }  

}  

//---------------------------------------------------------------------------------//  

// WIFI SETUP                                                                      //  

//---------------------------------------------------------------------------------//  

void WIFI_Conection(){  

  WiFi.mode(WIFI_STA);  

  WiFi.onEvent(ConnectedToAP_Handler, ARDUINO_EVENT_WIFI_STA_CONNECTED);  

  WiFi.onEvent(GotIP_Handler, ARDUINO_EVENT_WIFI_STA_GOT_IP);  

  WiFi.onEvent(WiFi_Disconnected_Handler, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);  

  WiFi.begin(ssid,wpa);  

  Serial.println("Connecting to WiFi Network ..");  

}  

  

void ConnectedToAP_Handler(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info) {  

  Serial.println("Connected To The WiFi Network"); isWiFiConnected = true;  

}  

   

void GotIP_Handler(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info) {  

  Serial.print("Local ESP32 IP: "); Serial.println(WiFi.localIP());  

}  

   

void WiFi_Disconnected_Handler(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info) {  

  Serial.println("Disconnected From WiFi Network"); isWiFiConnected = false;  

  /* Attempt Re-Connection*/ WiFi.begin(ssid, wpa);  

}  

  
