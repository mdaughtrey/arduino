// esp32io.com

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeMono18pt7b.h>
#include <Fonts/FreeMono24pt7b.h>
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeMonoBoldOblique18pt7b.h>
#include <Fonts/FreeMonoBoldOblique24pt7b.h>
#include <Fonts/FreeMonoBoldOblique9pt7b.h>
#include <Fonts/FreeMonoOblique12pt7b.h>
#include <Fonts/FreeMonoOblique18pt7b.h>
#include <Fonts/FreeMonoOblique24pt7b.h>
#include <Fonts/FreeMonoOblique9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBoldOblique12pt7b.h>
#include <Fonts/FreeSansBoldOblique18pt7b.h>
#include <Fonts/FreeSansBoldOblique24pt7b.h>
#include <Fonts/FreeSansBoldOblique9pt7b.h>
#include <Fonts/FreeSansOblique12pt7b.h>
#include <Fonts/FreeSansOblique18pt7b.h>
#include <Fonts/FreeSansOblique24pt7b.h>
#include <Fonts/FreeSansOblique9pt7b.h>
#include <Fonts/FreeSerif12pt7b.h>
#include <Fonts/FreeSerif18pt7b.h>
#include <Fonts/FreeSerif24pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/FreeSerifBold12pt7b.h>
#include <Fonts/FreeSerifBold18pt7b.h>
#include <Fonts/FreeSerifBold24pt7b.h>
#include <Fonts/FreeSerifBold9pt7b.h>
#include <Fonts/FreeSerifBoldItalic12pt7b.h>
#include <Fonts/FreeSerifBoldItalic18pt7b.h>
#include <Fonts/FreeSerifBoldItalic24pt7b.h>
#include <Fonts/FreeSerifBoldItalic9pt7b.h>
#include <Fonts/FreeSerifItalic12pt7b.h>
#include <Fonts/FreeSerifItalic18pt7b.h>
#include <Fonts/FreeSerifItalic24pt7b.h>
#include <Fonts/FreeSerifItalic9pt7b.h>
#include <Fonts/Org_01.h>
#include <Fonts/Picopixel.h>
#include <Fonts/Tiny3x3a2pt7b.h>
#include <Fonts/TomThumb.h>

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
    const __FlashStringHelper * label;
    const GFXfont *f;
}FontSel;

typedef struct
{
    char key;
    const __FlashStringHelper * help;
    //void (*help)();
    void (*fun)();
}Command;

const int PARAMBUF=32;

struct {
    uint8_t fontIndex;
    uint8_t size;
    char param[PARAMBUF];
    uint8_t paramIndex;
}config;

Command * commandset;
extern Command commands_main[];

FontSel fonts[] = {
    { F("Mono12pt7b"), &FreeMono12pt7b },
    { F("Mono18pt7b"), &FreeMono18pt7b },
//    { F("Mono24pt7b"), &FreeMono24pt7b },
    { F("Mono9pt7b"), &FreeMono9pt7b },
    { F("MonoBold12pt7b"), &FreeMonoBold12pt7b },
    { F("MonoBold18pt7b"), &FreeMonoBold18pt7b },
//    { F("MonoBold24pt7b"), &FreeMonoBold24pt7b },
    { F("MonoBold9pt7b"), &FreeMonoBold9pt7b },
    { F("MonoBoldOblique12pt7b"), &FreeMonoBoldOblique12pt7b },
    { F("MonoBoldOblique18pt7b"), &FreeMonoBoldOblique18pt7b },
//    { F("MonoBoldOblique24pt7b"), &FreeMonoBoldOblique24pt7b },
    { F("MonoBoldOblique9pt7b"), &FreeMonoBoldOblique9pt7b },
    { F("MonoOblique12pt7b"), &FreeMonoOblique12pt7b },
    { F("MonoOblique18pt7b"), &FreeMonoOblique18pt7b },
//    { F("MonoOblique24pt7b"), &FreeMonoOblique24pt7b },
    { F("MonoOblique9pt7b"), &FreeMonoOblique9pt7b },
    { F("Sans12pt7b"), &FreeSans12pt7b },
    { F("Sans18pt7b"), &FreeSans18pt7b },
//    { F("Sans24pt7b"), &FreeSans24pt7b },
    { F("Sans9pt7b"), &FreeSans9pt7b },
    { F("SansBold12pt7b"), &FreeSansBold12pt7b },
    { F("SansBold18pt7b"), &FreeSansBold18pt7b },
//    { F("SansBold24pt7b"), &FreeSansBold24pt7b },
    { F("SansBold9pt7b"), &FreeSansBold9pt7b },
    { F("SansBoldOblique12pt7b"), &FreeSansBoldOblique12pt7b },
    { F("SansBoldOblique18pt7b"), &FreeSansBoldOblique18pt7b },
//    { F("SansBoldOblique24pt7b"), &FreeSansBoldOblique24pt7b },
    { F("SansBoldOblique9pt7b"), &FreeSansBoldOblique9pt7b },
    { F("SansOblique12pt7b"), &FreeSansOblique12pt7b },
    { F("SansOblique18pt7b"), &FreeSansOblique18pt7b },
//    { F("SansOblique24pt7b"), &FreeSansOblique24pt7b },
    { F("SansOblique9pt7b"), &FreeSansOblique9pt7b },
    { F("Serif12pt7b"), &FreeSerif12pt7b },
    { F("Serif18pt7b"), &FreeSerif18pt7b },
//    { F("Serif24pt7b"), &FreeSerif24pt7b },
    { F("Serif9pt7b"), &FreeSerif9pt7b },
    { F("SerifBold12pt7b"), &FreeSerifBold12pt7b },
    { F("SerifBold18pt7b"), &FreeSerifBold18pt7b },
//    { F("SerifBold24pt7b"), &FreeSerifBold24pt7b },
    { F("SerifBold9pt7b"), &FreeSerifBold9pt7b },
    { F("SerifBoldItalic12pt7b"), &FreeSerifBoldItalic12pt7b },
    { F("SerifBoldItalic18pt7b"), &FreeSerifBoldItalic18pt7b },
//    { F("SerifBoldItalic24pt7b"), &FreeSerifBoldItalic24pt7b },
    { F("SerifBoldItalic9pt7b"), &FreeSerifBoldItalic9pt7b },
    { F("SerifItalic12pt7b"), &FreeSerifItalic12pt7b },
    { F("SerifItalic18pt7b"), &FreeSerifItalic18pt7b },
//    { F("SerifItalic24pt7b"), &FreeSerifItalic24pt7b },
    { F("SerifItalic9pt7b"), &FreeSerifItalic9pt7b },
    { F("Org_01"), &Org_01 },
    { F("Picopixel"), &Picopixel },
    { F("Tiny3x3a2pt7b"), &Tiny3x3a2pt7b },
    { F("TomThumb"), &TomThumb }
};

#define NUM_FONTS (sizeof(fonts)/sizeof(fonts[0]))


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

void redisplay()
{
    display.clearDisplay();
    display.setTextSize(config.size);
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0, SCREEN_HEIGHT-1);             // Start at top-left corner
    display.setFont(fonts[config.fontIndex].f);
    display.println(fonts[config.fontIndex].label);
    Serial.print(fonts[config.fontIndex].label);
    Serial.println(config.size);
    display.display();
}

void cmd_face_select(void)
{
    config.fontIndex++;
    config.fontIndex %= NUM_FONTS;
    redisplay();
}

void cmd_size_select(void)
{
    config.size++;
    config.size %= 4;
    redisplay();
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
    memset(&config, 0, sizeof(config));
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
