
![output](https://github.com/user-attachments/assets/57935ed7-35be-407e-9130-a2ebedb8cb19)

## ShowerThoughts on ESP8266

A simple, and probably inelegant, ESP8266 sketch that, on reset, grabs Reddit’s top /r/Showerthoughts posts (via ThingSpeak) and scrolls one random thought on a LCD display. Each reset advances a counter so you see a new thought next time.

Sorry for the low video brightness. The camera was struggling to capture the scrolling text with proper contrast.

**How it works:**
1. An IFTTT applet pushes top Showerthoughts into a ThingSpeak channel.  
2. On boot, the ESP fetches that channel’s full list and a second channel’s counter value `n`.  
3. It parses the JSON to the `n`th post and scrolls it across the LCD.  
4. Finally, it increments `n` back to ThingSpeak for the next reset.

Fill in your credentials in `secrets.h` and enjoy random sparks of wisdom, one reset at a time!
