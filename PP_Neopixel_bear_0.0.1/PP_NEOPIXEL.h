#ifndef pp_neopixel_h
#define pp_neopixel_h

#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

byte red, green, blue;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, 2, NEO_GRB + NEO_KHZ800);

void pp_neopixel_setup() {
  EEPROM.begin(512);
  // read previouse value 
  red = EEPROM.read(0);
  green = EEPROM.read(1);
  blue = EEPROM.read(2);
  // debug on serial
  Serial.print(" R ");Serial.print(red);
  Serial.print(" - G ");Serial.print(green);
  Serial.print(" - B ");Serial.println(blue);
  // update color
  strip.begin();
  strip.show();
  for (int i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(red,   green,   blue));
  }
  strip.show();  
}


void pp_neopixel_change_color(byte red, byte green, byte blue) {
  // write on eeprom
  EEPROM.write(0, red);
  EEPROM.write(1, green);
  EEPROM.write(2, blue);
  EEPROM.commit();
  // debug on serial
  Serial.print(" R ");Serial.print(red);
  Serial.print(" - G ");Serial.print(green);
  Serial.print(" - B ");Serial.println(blue);
  // update color
  for (int i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
  }
  strip.show();    
}


void pp_neopixel_loop() {
  
}

#endif
