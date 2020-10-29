/* Copyright (c) 2017 LeRoy Miller
 *  
 *  This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses>

 Notes:
 * Modes: (Google Trigger is "Eric $ mode" where $ is the mode
 * word below.
 * 
 * angry - red
 * happy/norma - blue (normal)
 * scann - greed (scanner)
 * black/charg - (charge)charging no led off. (not really charging)
 * party - rainbow party mode - change to another mode a little slow
 */

#include "src/arduino_dweet/Dweet.h" //https://github.com/quentinpigne/arduino-dweet.io
#include <ESP8266WiFi.h>
#include "src/Adafruit_NeoPixel/Adafruit_NeoPixel.h"
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "src/WiFiManager/WiFiManager.h"         //https://github.com/tzapu/WiFiManager

String mood;
int brightness = 80; //max brightness setting
int minbrightness = 50; //min brightness for breathe effect
int fade = 150; //breathe effect starting brightness
int step = 5; //5 seems to work best for this application
int wait = 0; //because we need to feed the wdt and yield the process zero seems to work best here
int fadedir = 1; //count up

unsigned long previousMillis = 0;
const long interval = 500; //check every 1/2 second

WiFiClient espClient;
Dweet dweet(espClient); 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, D2, NEO_GRB + NEO_KHZ800);


void setup() {
   pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  digitalWrite(BUILTIN_LED, HIGH);
  Serial.begin(9600);
  WiFiManager wifiManager;
     wifiManager.autoConnect("AutoConnectAP");
  strip.begin();
  strip.show();
  strip.setBrightness(brightness);

}

void loop() {
   unsigned long currentMillis = millis();
   if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
  getMood();
   }
  setColor();
  ESP.wdtFeed();
  yield();
  //delay(100); 
}

void getMood() {
 // Get latest dweet !
  char* res = dweet.get_latest("ericcube");
  mood = (String)res;
  mood.remove(0,134); //126
  mood.remove(5);
  Serial.println(mood);
  //  ESP.wdtFeed();
  //yield();
}

void setColor() {
  if (mood == "black" || mood == "charg") {colorDisplay(strip.Color(0,0,0),wait);
} else if (mood == "angry") { colorDisplay(strip.Color(255,0,0),wait);
} else if (mood == "scann") { colorDisplay(strip.Color(0, 255, 0), wait);
} else if (mood == "norma" || mood == "happy") { colorDisplay(strip.Color(0,0,155),wait);
} else if (mood == "party") {partyMood(5);}
 //ESP.wdtFeed();
 //yield();

}

void colorDisplay(uint32_t c, uint8_t wait) {
strip.setBrightness(fade);
strip.setPixelColor(0, c);
strip.show();


if (fadedir == 0) {fade = fade - step; } else {fade = fade + step; }
if (fade <= minbrightness) {fadedir = 1; fade = minbrightness;}
if (fade >= brightness) {fadedir = 0; fade = brightness; }
//delay(wait);
 //ESP.wdtFeed();
  //yield();
}

void partyMood(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
