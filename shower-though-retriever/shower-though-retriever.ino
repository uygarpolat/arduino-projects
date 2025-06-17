#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ThingSpeak.h>
#include "secrets.h"

// LCD address and geometry and library initialization
const byte lcdAddr = 0x27;
const byte lcdCols = 16;
const byte lcdRows = 2;

LiquidCrystal_I2C lcd(lcdAddr, lcdCols, lcdRows);

// WiFi Parameters
const char* ssid = "";
const char* password = "";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  lcd.print("Shower Thought:");
  lcd.setCursor(0, 1);
  lcd.print("   Retrieving");
  delay(2000);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient ThoughtHTTP;
    HTTPClient CounterHTTP;
	ThoughtHTTP.begin("http://api.thingspeak.com/channels/" + String(THOUGHTS_CHANNEL) + "/fields/1.json?api_key=" + THOUGHTS_READ_KEY);
	CounterHTTP.begin("http://api.thingspeak.com/channels/" + String(COUNTER_CHANNEL) + "/fields/2.json?api_key=" + COUNTER_READ_KEY + "&results=1"
);
    int httpCodeThought = ThoughtHTTP.GET();
    int httpCodeCounter = CounterHTTP.GET();

    //Check the returning code
    if (httpCodeThought > 0 && httpCodeCounter > 0) {

      // Parsing "Counter"
      const size_t bufferSizeCounter = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(8) + 1267;
      DynamicJsonBuffer jsonBufferCounter(bufferSizeCounter);
      JsonObject& rootCounter = jsonBufferCounter.parseObject(CounterHTTP.getString());
      JsonObject& feedsCounter0 = rootCounter["feeds"][0];
      int counter = feedsCounter0["field2"]; // "33"

      Stream& json2 = ThoughtHTTP.getStream();
      json2.find("\"feeds\"");
      json2.find("[");
      const char* ShowerThought;

      for (int i = 1; i <= counter; i++) {
        DynamicJsonBuffer jb(2048);
        JsonObject &obj = jb.parseObject(json2);

        if (i == counter) {
          ShowerThought = obj["field1"];
        }

        json2.findUntil(",", "]");
      }

      if (!ShowerThought) {
        ShowerThought = "Oops! We seem to be fresh out of ShowerThoughts on file. Please encourage people to daydream, and inquire again later.";
      }

      String Line1_fixed = "Shower Thought:";
      String Line2_scrolling_base = ShowerThought;

      lcd.clear();
      lcd.print(Line1_fixed);
      delay(500);

      String Line2_scrolling =  "                ";
      Line2_scrolling += Line2_scrolling_base;

      int Line2_length = Line2_scrolling.length(); // Length of our string (including the sixteen spaces that we added in front).
      int Line2_ChunkNumber = floor(Line2_length / 16 + 2); // Number of sixteen-character chunks in the string, plus one for the last chunk that is less than sixteen characters long.
      int Line2_LastChunkLength = Line2_length - ((Line2_ChunkNumber - 2) * 16); // Length of the last chunk, which is usually less than sixteen.
      int Line2_ChunkLength; // Will be defined as 16 below, which is the usual length of chunks. But it will be changed if the last chunk is smaller in length.
      if (Line2_LastChunkLength == 0) { // If the string has a character length of a  multiple of sixteen, Line2_ChunkNumber is reduced by one, in order to prevent redundant for cycles.
        Line2_ChunkNumber--;
      }

      String chunk[Line2_ChunkNumber]; // chunk[7]
      String RandomNotes[] = {"WHAT AN INSIGHT!", "    TOO GOOD!", "   OF COURSE!", "     WHAT!!", "WHO THINKS THAT!", "   MUCH DEEP!", "    TOO DEEP!", "   CRAZINESS!", "    SO TRUE!", "     WOAH!!", "     JESUS!", "      HA??", "   I GUESS...", "  I SUPPOSE...", "   SERIOUSLY?", "HAHAHAhahaha....", "SAID THE MADMAN!"};

      for (int j = 0; j < Line2_ChunkNumber - 1; j++) { // Assign all chunks inside chunk[].
        chunk[j] = Line2_scrolling.substring(j * 16 , j * 16 + 16);
      }

      for (int k = 0; k < Line2_ChunkNumber - 1; k++) {
        lcd.clear();
        lcd.print(Line1_fixed);
        lcd.setCursor(0, 1);
        lcd.print(chunk[k]); // Display the string with the chunk number "k", starting from (0,1). This is the entire visible screen area of line 2 of the LCD.
        lcd.print(chunk[k + 1]); // Print the string with the chunk number "k+1", starting from (16,1). This is the area that is immediately to the right of the visible area of line 2 of the LCD.
        if (k != Line2_ChunkNumber - 2) { // If we are not dealing with the last chunk, set the number of iterations in the next for cycle to 16. If it is the last chunk and if the length of the last chunk isn't zero, set the number of iterations in the next for cycle to size of last chunk.
          Line2_ChunkLength = 16;
        }
        else if (Line2_LastChunkLength != 0) {
          Line2_ChunkLength = Line2_LastChunkLength;
        }

        for (int i = 1; i < Line2_ChunkLength + 1; i++) {
          lcd.scrollDisplayLeft();
          lcd.setCursor(i, 0);
          lcd.print(Line1_fixed);
          delay(150);
        }
      }

      lcd.clear();
      lcd.print(RandomNotes[random(17)]);
      lcd.setCursor(0, 1);
      lcd.print("   PLEASE HOLD");

      WiFiClient client;
      ThingSpeak.begin(client);
	  ThingSpeak.writeField(COUNTER_CHANNEL, 2, counter + 1, COUNTER_WRITE_KEY);

      lcd.clear();
      delay(500);

      for (int m = 15; m > 0; m--) {
        lcd.clear();
        lcd.print("HIT RST FOR MORE");
        lcd.setCursor(0, 1);
        lcd.print(" IN ");
        lcd.print(m);
        lcd.print(" SECONDS.");
        delay(1000);
      }
      lcd.clear();
      lcd.print("HIT RST FOR MORE");
      lcd.setCursor(0, 1);
      lcd.print("SHOWER THOUGHTS");

    } else {
      Serial.println("ThingSpeak connection problem.");
      lcd.clear();
      lcd.print("CAN'T REACH SITE");
      lcd.setCursor(0, 1);
      lcd.print(" PLEASE HIT RST");
    }

    CounterHTTP.end();
    ThoughtHTTP.end();

  } else {
    Serial.println("Internet connection problem.");
    lcd.clear();
    lcd.print("CAN'T REACH WEB");
    lcd.setCursor(0, 1);
    lcd.print(" PLEASE HIT RST");
  }
}

void loop() {
}
