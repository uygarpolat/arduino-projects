#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <ESP8266WiFi.h>

#include "secrets.h"
// #include "ThingSpeak.h" 
char ssid[] = SECRET_SSID; // your network SSID (name)
char pass[] = SECRET_PASS; // your network password
int keyIndex = 0; // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
const char * myReadAPIKey = SECRET_READ_APIKEY_COUNTER;

// LCD address and geometry and library initialization
const byte lcdAddr = 0x27;  // Address of I2C backpack
const byte lcdCols = 16;    // Number of character in a row
const byte lcdRows = 2;     // Number of lines
//const byte lcdAddr = 0x3F;  // Address of I2C backpack
//const byte lcdCols = 20;    // Number of character in a row
//const byte lcdRows = 4;     // Number of lines

LiquidCrystal_I2C lcd(lcdAddr, lcdCols, lcdRows);

String alwaysDisplay = "Distance:"; //The message that will always be displayed on row 1.

unsigned long LastMillis = 0;

const int trigPin = D6;
const int echoPin = D5;

//define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

void setup() {
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  lcd.clear(); //Clear the LCD screen, just in case!

  WiFi.mode(WIFI_STA);

  Serial.println();

  long internetConnectionDuration = millis();

  WiFi.begin(ssid, pass);

  Serial.print("Connecting");
  lcd.print("Connecting!");
  lcd.setCursor(0, 1);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    lcd.print(".");
  }
  Serial.println();

  Serial.print("Connected to ");
  Serial.print(ssid);
  Serial.print(" in ");
  Serial.print((millis() - internetConnectionDuration) / 1000);
  Serial.print(" seconds. IP address: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.print("Connection: OK!");

  delay (2000);

  // ThingSpeak.begin(client);  // Initialize ThingSpeak

  UltrasonicFunction ();
  DisplayFunction ();
  LastMillis = millis();
}

void loop() {
  unsigned long CurrentMillis = millis();
  if (CurrentMillis - LastMillis > 1000) {
    UltrasonicFunction ();
    DisplayFunction ();
    LastMillis = millis();
  }
}

void DisplayFunction () {
  lcd.clear();
  lcd.print(alwaysDisplay);
  lcd.setCursor(0, 1);
  lcd.print(int(distanceCm));
}

void UltrasonicFunction () {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance
  distanceCm = duration * SOUND_VELOCITY / 2;

  // Prints the distance on the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
}