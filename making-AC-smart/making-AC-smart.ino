#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define BLYNK_PRINT Serial

#include "DHT.h"
#include <ArduinoOTA.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Math.h>
#include <SimpleTimer.h>

#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();

unsigned long myChannelNumber = 00000;
const char * myWriteAPIKey = "";


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "";
char pass[] = "";

// LCD address and geometry and library initialization
const byte lcdAddr = 0x27;  // Address of I2C backpack
const byte lcdCols = 16;    // Number of character in a row
const byte lcdRows = 2;     // Number of lines
//const byte lcdAddr = 0x3F;  // Address of I2C backpack
//const byte lcdCols = 20;    // Number of character in a row
//const byte lcdRows = 4;     // Number of lines

LiquidCrystal_I2C lcd(lcdAddr, lcdCols, lcdRows);

#define DHTPIN D4
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTPIN2 D5
//#define DHTTYPE2 DHT22   // DHT 22  (AM2302), AM2321

const boolean IS_METRIC = true;

DHT dht(DHTPIN, DHTTYPE);
//DHT dht2(DHTPIN2, DHTTYPE2);

unsigned long timeSinceJoystick;
unsigned long timeSinceACturnedOn;
unsigned long lcdUpdateDuration;

float humidityFloat;
int humidity;
float temperatureFloat;
int temperature;
float humidityFloat2;
int humidity2;
float temperatureFloat2;
int temperature2;
float minTemp = 24;
float maxTemp = 35;

int opened = 0;
int closed = 120;

int Xcoordinate = 2000;
int Ycoordinate = 2000;

int ACcounter = 1;
int ACduration = 600; // in seconds.
int enterLCDupdateTimer = 1;
int screenOFF = 10; // in seconds.

unsigned long iterationCount = 0;
int counterCounter = 0;

SimpleTimer timer;

int buttonState = 0;
int buttonState1 = 0;
int buttonState2 = 0;
int buttonState3 = 0;
int buttonState4 = 0;
int buttonState5 = 0;

unsigned long timeSinceAC = 0;

String Buffer33;
String Buffer44;

unsigned long t1;
unsigned long t2;
unsigned long t3;
unsigned long t4;

BLYNK_WRITE(V22) {
  buttonState = param.asInt();
  mySwitch.setPulseLength(213);
  if (buttonState == 0)
    mySwitch.send(7704710, 24);
  else
    mySwitch.send(7704711, 24);
}

BLYNK_WRITE(V24) {
  buttonState1 = param.asInt();
  mySwitch.setPulseLength(185);
  if (buttonState1 == 0)
    mySwitch.send(5592380, 24);
  else
    mySwitch.send(5592371, 24);
}

BLYNK_WRITE(V25) {
  buttonState2 = param.asInt();
  mySwitch.setPulseLength(185);
  if (buttonState2 == 0)
    mySwitch.send(5592524, 24);
  else
    mySwitch.send(5592515, 24);
}

BLYNK_WRITE(V26) {
  buttonState3 = param.asInt();
  mySwitch.setPulseLength(185);
  if (buttonState3 == 0)
    mySwitch.send(5592844, 24);
  else
    mySwitch.send(5592835, 24);
}

BLYNK_WRITE(V31) {
  buttonState4 = param.asInt();
  mySwitch.setPulseLength(185);
  if (buttonState4 == 0)
    mySwitch.send(4545804, 24);
  else
    mySwitch.send(4545795, 24);
}

BLYNK_WRITE(V30) {
  buttonState5 = param.asInt();
  mySwitch.setPulseLength(185);
  if (buttonState5 == 0) {
    mySwitch.send(4551948, 24);
    Blynk.virtualWrite(V24, LOW);
    Blynk.virtualWrite(V26, LOW);
    Blynk.virtualWrite(V25, LOW);
  } else {
    mySwitch.send(4551939, 24);
    Blynk.virtualWrite(V24, HIGH);
    Blynk.virtualWrite(V26, HIGH);
    Blynk.virtualWrite(V25, HIGH);
  }
}

BLYNK_WRITE(V40) {
  temperatureFloat2 = param.asFloat();
}

BLYNK_WRITE(V41) {
  humidityFloat2 = param.asFloat();
}

void setup() {

  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);

  // Transmitter is connected to Arduino Pin #0
  mySwitch.enableTransmit(0);  // Pin 0 is D3 on ESP8266
  //mySwitch.setRepeatTransmit(20);
  mySwitch.setPulseLength(213); // Pulse Length for AC plug. The value for Etekcity plugs is 185.

  Blynk.syncVirtual(V22);
  //  Blynk.syncVirtual(V24);
  //  Blynk.syncVirtual(V25);
  //  Blynk.syncVirtual(V26);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  delay(5000);
  readTempHumi();
  delay(5000);


  //  delay(3000);
  //  uploadTempAndHumiToThingSpeak ();
  //  delay(3000);

  timer.setInterval(15000, readTempHumi);
  //timer.setInterval(300000, uploadTempAndHumiToThingSpeak);

  ArduinoOTA.begin();
}

void loop() {
  Blynk.run();
  timer.run();
  ArduinoOTA.handle();

  if (millis() / 1000 - timeSinceACturnedOn < ACduration && ACcounter == 0) {
    mySwitch.send(7704711, 24);
    if (enterLCDupdateTimer == 1) {
      lcdUpdateDuration = millis();
      enterLCDupdateTimer = 0;
    }
    if (lcdUpdateDuration + 1000 <= millis()) {
      updateLCD();
      lcdUpdateDuration = millis();
    }
  } else if (temperatureFloat >= maxTemp && ACcounter == 0) {
    timeSinceACturnedOn = millis() / 1000;
    ACcounter = 0;
  } else {
    mySwitch.send(7704710, 24);
    ACcounter = 1;
    if (enterLCDupdateTimer == 0)
      updateLCD();
    enterLCDupdateTimer = 1;
  }
}

void readTempHumi () {
  t1 = millis();

  int stopper = 0;
  while (stopper == 0) {
    // read values from the sensor
    humidityFloat = dht.readHumidity();
    // humidity = roundf(humidityFloat);
    temperatureFloat = dht.readTemperature(!IS_METRIC);
    // temperature = roundf(temperatureFloat);

//    // read values from the sensor
//    humidityFloat2 = dht2.readHumidity();
//    // humidity2 = roundf(humidityFloat2);
//    temperatureFloat2 = dht2.readTemperature(!IS_METRIC);
//    // temperature2 = roundf(temperatureFloat2);

    if (humidityFloat > 0 && temperatureFloat > 0)
      stopper = 1;
  }

  Serial.print("Temp: ");
  Serial.print(temperatureFloat, 1);
  Serial.println(" *C, ");
  Serial.print("Humi: ");
  Serial.print(humidityFloat, 1);
  Serial.println("%");
  Serial.print("Temp2: ");
  Serial.print(temperatureFloat2, 1);
  Serial.println(" *C, ");
  Serial.print("Humi2: ");
  Serial.print(humidityFloat2, 1);
  Serial.println("%");
  Serial.println("------------------");

  updateLCD ();

  char Buffer[16];
  dtostrf(temperatureFloat, 1, 1, Buffer);
  Blynk.virtualWrite(V20, Buffer);
  char Buffer2[16];
  dtostrf(humidityFloat, 1, 1, Buffer2);
  Blynk.virtualWrite(V21, Buffer2);
  char Buffer3[16];
  dtostrf(temperatureFloat2, 1, 1, Buffer3);
  Blynk.virtualWrite(V28, Buffer3);
  Buffer33 = Buffer3;
  char Buffer4[16];
  dtostrf(humidityFloat2, 1, 1, Buffer4);
  Blynk.virtualWrite(V29, Buffer4);
  Buffer44 = Buffer4;

  if (temperatureFloat >= maxTemp && ACcounter == 1) {
    timeSinceACturnedOn = millis() / 1000;
    ACcounter = 0;
  }

  t2 = millis() - t1;
  Blynk.virtualWrite(V32, t2);

}

void updateLCD () {
  lcd.clear();
  lcd.setCursor(0, 0);

  if (ACcounter == 1) {
    lcd.print("Temp:");
    lcd.print(temperatureFloat, 1);
    //    lcd.print((char)223);
    //    lcd.print("C");
    lcd.print("/");
    lcd.print(temperatureFloat2, 1);
    lcd.setCursor(0, 1);
    lcd.print("Humi:");
    lcd.print(humidityFloat, 1);
    //    lcd.print("%");
    lcd.print("/");
    lcd.print(humidityFloat2, 1);

    //    int valLen = 0;
    //
    //    if (iterationCount > 9999)
    //      valLen = 5;
    //    else if (iterationCount > 999)
    //      valLen = 4;
    //    else if (iterationCount > 99)
    //      valLen = 3;
    //    else if (iterationCount > 9)
    //      valLen = 2;
    //    else
    //      valLen = 1;
    //
    //    lcd.setCursor(16 - valLen, 0);
    //    lcd.print(iterationCount);

  } else {
    lcd.print("Tmp/Mx:");
    lcd.print(temperatureFloat, 1);
    lcd.print("/");
    lcd.print(maxTemp, 1);
    lcd.setCursor(0, 1);
    lcd.print("AC off in ");
    lcd.print(ACduration - (millis() / 1000) + timeSinceACturnedOn);
    lcd.print(".");

    Serial.print("AC turned on, it will turn off in ");
    Serial.print(ACduration - (millis() / 1000) + timeSinceACturnedOn);
    Serial.println(" seconds.");
  }
}

// Not used in this project, but can be used to upload data to ThingSpeak.
void uploadTempAndHumiToThingSpeak () {
  //  ThingSpeak.setField(1, Buffer33);
  //  ThingSpeak.setField(2, Buffer44);
  //
  //  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  t3 = millis();

  const char* host = "api.thingspeak.com";

  const char* THINGSPEAK_API_KEY = ""; // Write key.

  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // We now create a URI for the request
  String url = "/update?api_key=";
  url += THINGSPEAK_API_KEY;
  url += "&field1=";
  url += Buffer33; // was url += String(Buffer33);
  url += "&field2=";
  url += Buffer44;

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  t4 = millis() - t3;
  Blynk.virtualWrite(V33, t4);
}
