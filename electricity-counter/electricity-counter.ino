#include <ESP8266WiFi.h>

#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

int LocalNumber = 0;
unsigned long LastMillis = 0;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
const char * myReadAPIKey = SECRET_READ_APIKEY_COUNTER;

const int ledPin = D1;   //the number of the LED pin
const int ldrPin = A0;  //the number of the LDR pin

void setup() {
  Serial.begin(115200);  // Initialize serial
  pinMode(ledPin, OUTPUT);  //initialize the LED pin as an output
  pinMode(ldrPin, INPUT);   //initialize the LDR pin as an input

  WiFi.mode(WIFI_STA);

  Serial.println();

  long internetConnectionDuration = millis();

  WiFi.begin(ssid, pass);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected to ");
  Serial.print(ssid);
  Serial.print(" in ");
  Serial.print((millis() - internetConnectionDuration) / 1000);
  Serial.print(" seconds. IP address: ");
  Serial.println(WiFi.localIP());

  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
  LightONorOFF ();
  GetThingspeak ();
}

void GetThingspeak () {

  unsigned long CurrentMillis = millis();

  if (CurrentMillis - LastMillis > 300000) {

    ThingSpeak.writeField(myChannelNumber, 1, LocalNumber, myWriteAPIKey);

    LocalNumber = 1; // Would have been 0, but chose 1 in order to compensate for one lost measurement due to ThingSpeak Read and Write process time that usually takes around one seconds.
    LastMillis = millis();
  }
}

void LightONorOFF () {
  int ldrStatus = analogRead(ldrPin); //read the status of the LDR value

  if (ldrStatus > 300) {
    digitalWrite(ledPin, HIGH); //turn LED on
    LocalNumber++;
    delay(10);
    digitalWrite(ledPin, LOW);
    delay(100);
  }
}