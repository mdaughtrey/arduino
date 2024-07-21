// esp32io.com

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define FSH(s) reinterpret_cast<const __FlashStringHelper *>(s)

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


typedef struct
{
    char key;
    const __FlashStringHelper * help;
    //void (*help)();
    void (*fun)();
}Command;

const int PARAMBUF=32;

struct {
    uint8_t face;
    uint8_t size;
    char param[PARAMBUF];
    uint8_t paramIndex;
}config;

Command * commandset;
extern Command commands_main[];

void handleCommand(uint8_t command);

const char help_help[] PROGMEM = "Help";
const char help_faceselect[] PROGMEM = "Face Select";
const char help_facelist[] PROGMEM = "List Faces";
const char help_sizeselect[] PROGMEM = "Size Select";
const char help_showconfig[] PROGMEM = "Show Configuration";
const char help_reset[] PROGMEM = "Reset";
const char dashes[] PROGMEM="-------------------------------------------------------\r\n";

void cmd_help()
{
    Serial.printf("%s\r\n", FSH(dashes));
    for (Command * iter = commandset; iter->key != '&'; iter++)
    {
        Serial.printf("%c: %s\r\n", iter->key, iter->help);
    }
}

void cmd_face_select(void)
{
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Hello, world!"));
  display.display();
}

void cmd_size_select(void)
{
}

void cmd_face_list(void)
{
}

void cmd_show_config(void)
{
}

void cmd_reset(void)
{
    ESP.restart();
}

Command commands_main[] = {
{'?',FSH(help_showconfig), [](){ cmd_show_config();}},
{'f',FSH(help_faceselect), [](){ cmd_face_select();}},
{'F',FSH(help_facelist), [](){ cmd_face_list();}},
{'h',FSH(help_help), [](){ cmd_help();}},
{'s',FSH(help_sizeselect), [](){ cmd_size_select(); }},
{' ',FSH(help_reset), [](){ cmd_reset(); }},
{'&',FSH(help_reset), [](){ cmd_reset(); }}
};

void handleCommand(uint8_t command)
{
    for (Command * iter = commandset; iter->key != '&'; iter++)
    {
        if (iter->key == command)
        {
            iter->fun();
            return;
        }
    }
}

void setup(void)
{
    commandset = commands_main;
    Serial.begin(115200);
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }
    display.display();
}


void loop(void)
{
    if (Serial.available())
    {
        handleCommand(Serial.read());
    }
}
