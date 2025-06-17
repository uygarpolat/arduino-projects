# ESP8266 Electricity Meter Pulse Counter → ThingSpeak

A simple ESP8266-based pulse counter for your electricity meter’s LED indicator, with usage logging to ThingSpeak.

My electricity provider’s consumption interface wasn’t flexible enough, so I went for the jugular and started retrieving real-time usage by attaching a photoresistor to my meter’s light that blinks 1000 times per kilowatt-hour.

The ESP8266 processes these pulses and sends the count to a [ThingSpeak](https://thingspeak.mathworks.com/) channel for logging and monitoring every 5 minutes.

## Hardware

- **ESP8266 board** (e.g. NodeMCU, Wemos D1 Mini)  
- **LDR (photoresistor)**
- **Access to your meter’s LED indicator** (ensure safe and legal mounting of the LDR sensor)
