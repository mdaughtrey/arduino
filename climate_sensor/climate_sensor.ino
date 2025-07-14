#include <SPI.h>
#include <Wire.h>
//#include <Arduino.h>

//#define TFT_MOSI 19
#define TFT_MOSI 19
#define TFT_SCLK 18

#define TFT_CS   5 // Chip select for display
#define TFT_DC   16 // Data/Command
#define TFT_RST  23 // Reset
#define TFT_BL   4  // Backlight

#include <TFT_eSPI.h> // Make sure TFT_eSPI is installed in Arduino IDE
#include </home/mattd/Arduino/libraries/Adafruit_GFX_Library/Fonts/FreeSansBold12pt7b.h>

TFT_eSPI tft = TFT_eSPI(135,240);

void setup() {
  Serial.begin(115200);
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH); // Turn on backlight
  tft.init();
  tft.setRotation(1); // Adjust rotation as needed
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
}

void loop() {
  tft.setCursor(0, 0);
  tft.setTextFont(1);
  tft.println("Hello, World!");
  tft.setCursor(0, 20);
  tft.setTextFont(2);
  tft.println("TTGO T-Display");
  tft.setCursor(0, 50);
  tft.setTextFont(4);
  tft.println("ESP32");
  delay(2000);
  tft.fillScreen(TFT_BLACK);
  delay(500);
}
