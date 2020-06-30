#include <SoftwareSerial.h>

int soundSensorPin = 12;  // define D0 Sensor Interface
int val = 0;              // define numeric variables val
int wifiRxPin = 2; //ESP8266 RX
int wifiTxPin = 3; //ESP8266 TX

const int SAMPLE_TIME = 400;
const int LOUD_THRESHOLD = 0;
unsigned long millisCurrent;
unsigned long millisLast = 0;
unsigned long millisElapsed = 0;
int sampleBufferValue = 0;
int soundValue;

String wifiSSID = "SSIDNAME";
String wifiPassword = "SSIDPASSWORD";
String ipThingSpeak = "184.106.153.149";

SoftwareSerial esp(wifiRxPin, wifiTxPin);

void setup() {
  ConnectToWifi();
  pinMode (soundSensorPin, INPUT) ;// output interface D0 is defined sensor
  Serial.begin (9600);
}

void loop() {
  millisCurrent = millis();
  millisElapsed = millisCurrent - millisLast;
  if (digitalRead(soundSensorPin) == 0) {
    sampleBufferValue = sampleBufferValue + 10;
  }

  if (millisElapsed > SAMPLE_TIME) {
    Serial.println(sampleBufferValue);
    if (sampleBufferValue > LOUD_THRESHOLD) {
      Serial.println("A Sound!!!");
      SendData(sampleBufferValue);
    }
    sampleBufferValue = 0;
    millisLast = millisCurrent;
  }
}

void SendData(int value) {
  esp.println("AT+CIPSTART=\"TCP\",\"" + ipThingSpeak + "\",80"); // Connecting to Thingspeak
  if (esp.find("Error")) {
    Serial.println("AT+CIPSTART Error");
  }

  String dataToSend = "GET https://api.thingspeak.com/update?api_key=YOURCHANNELAPIKEY";
  dataToSend += "&field1=";
  dataToSend += String(value);
  dataToSend += "\r\n\r\n";
  esp.print("AT+CIPSEND=");                                   // Setting dataLength to send ESP
  esp.println(dataToSend.length() + 2);
  delay(200);
  if (esp.find('>')) {
    esp.print(dataToSend);                                    // Sending the data
    Serial.println(dataToSend);
    Serial.println("Data sent");
    delay(200);
  }
  Serial.println("Connection closed.");
  esp.println("AT+CIPCLOSE");                                 // Closing the connection
}

void ConnectToWifi() {
  Serial.begin(9600);
  esp.begin(9600);
  esp.println("AT");
  while (!esp.find("OK")) {
    esp.println("AT");
    Serial.println("Can't find ESP8266.");
  }
  esp.println("AT+CWMODE=1");
  esp.println("AT+CWJAP=\"" + wifiSSID + "\",\"" + wifiPassword + "\"");
  while (!esp.find("OK"));
  delay(1000);
}