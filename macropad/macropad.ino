// esp32io.com
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/jtag-debugging/index.html
// ~/.arduino15/packages/esp32/tools/openocd-esp32/v0.12.0-esp32-20240318/share/openocd/scripts
// ~/.arduino15/packages/esp32/tools/openocd-esp32/v0.12.0-esp32-20240318/share/openocd/scripts/interface/raspberrypi-native.cfg
// ~/.arduino15/packages/esp32/tools/openocd-esp32/v0.12.0-esp32-20240318/share/openocd/scripts/interface/raspberrypi-gpio-connector.c
// https://cdn-learn.adafruit.com/downloads/pdf/adafruit-optical-fingerprint-sensor.pdf

// ESP32 Pin  | JTAG Signal | RPI5 GPIO | RPI5 PIN |
// MTDO/GPIO15 TDO          | 9         | 21       |
// MTDI/GPIO12 TDI          | 10        | 19       |
// MTCK/GPIO13 TCK          | 11        | 23       |
// MTMS/GPIO14 TMS          | 8         | 24       |

// Fingerprint Reader   | ESP32 Pin
// Red V+               | 19
// Yellow Tx            | 27
// White Rx             | 28
// Black Ground         | 38
// Blue TCH -- not used
// Green UA -- not used

//#define DISPLAY
#undef DISPLAY
//#undef FINGER
#define FINGER

// #define RX2 27  // 16
// #define TX2 28  // 17

#include <Arduino.h>
#include <Wire.h>
#ifdef DISPLAY
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#endif // DISPLAY
#include <Preferences.h>
//#include <Keyboard.h>
#include <ArduinoYaml.h>
#ifdef FINGER
#include <Adafruit_Fingerprint.h>
#endif // FINGER

//#include <Fonts/FreeSans9pt7b.h>
#ifdef DISPLAY
#include <Fonts/FreeSans12pt7b.h>
#endif // DISPLAY

#include <string>

#define NUM_KEYS 8
#define KEYSTORE_NAME "kv"

#define KEYSTORE_RW false
#define KEYSTORE_RO true

#define KEYSTORE_LABEL_MAXLEN 32
#define KEYSTORE_MACRO_MAXLEN 64
#define PARAM_LEN KEYSTORE_MACRO_MAXLEN * 4;

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

//ktypedef struct
//k{
//k    const __FlashStringHelper * label;
//k    const GFXfont *f;
//k}FontSel;

typedef struct
{
    char key;
    const __FlashStringHelper * help;
    //void (*help)();
    void (*fun)();
}Command;


//struct {
////    uint8_t fontIndex;
////    uint8_t size;
//    uint8_t param[PARAM_LEN];
//    uint8_t paramIndex;
//    int32_t numericParam;
//    bool accumParameter;
//}config;
int32_t numericParameter;
#ifdef FINGER
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);
#endif // FINGER

#ifdef DISPLAY
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#endif // DISPLAY
//Keyboard kb();
Command * command_set;
extern Command commands_main[];
extern Command commands_fingerprint[];

//jFontSel fonts[] = {
//j    { F("Sans9pt7b"), &FreeSans9pt7b },
//j    { F("Sans12pt7b"), &FreeSans12pt7b }
//j};
//j
//j#define NUM_FONTS (sizeof(fonts)/sizeof(fonts[0]))


void handleCommand(uint8_t command);
void cmd_help();
void redisplay();
void cmd_reset(void);
void cmd_clear_keystore(void);
void cmd_clear_numeric_parameter(void);
void cmd_test_key(void);
void cmd_upload_keydefs(void);
void cmd_show_config(void);
void handleCommand(uint8_t command);
void key_store_init(Preferences ks);
void cmd_serial2_test_string(void);
void setup(void);
#ifdef FINGER
void fingerprint_error(int8_t);
void cmd_fingerprint_init();
void cmd_fingerprint_menu(void);
void cmd_fingerprint_ledtest(void);
void cmd_fingerprint_delete(int32_t);
void cmd_fingerprint_enroll();
#endif // FINGER
void loop(void);

//const char dashes[] PROGMEM="-------------------------------------------------------\r\n";

void cmd_help()
{
    Serial.println(F("-------------------------------------------------------"));
    for (Command * iter = command_set; iter->key != '&'; iter++)
    {
        Serial.printf(F("%c: %s\r\n"), iter->key, iter->help);
    }
}

void redisplay()
{
#ifdef DISPLAY
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0, SCREEN_HEIGHT-1);             // Start at top-left corner
    display.setFont(&FreeSans12pt7b);
    display.display();
#endif // DISPLAY
}

//void cmd_face_select(void)
//{
//    config.fontIndex++;
//    config.fontIndex %= NUM_FONTS;
//    redisplay();
//}
//
//void cmd_size_select(void)
//{
//    config.size++;
//    config.size %= 4;
//    redisplay();
//}
//
//void cmd_face_list(void)
//{
//}
//
void cmd_show_config(void)
{
    Serial.printf(F("parameter %d\r\n"), numericParameter);
#ifdef FINGER
    Serial.println(F("Reading sensor parameters"));
    finger.getParameters();
    Serial.printf(F("Status: 0x%02x "), finger.status_reg);
    Serial.printf(F("Sys ID: 0x%02x "), finger.system_id);
    Serial.printf(F("Capacity: %u "), finger.capacity);
    Serial.printf(F("Security level: %d "), finger.security_level);
    Serial.printf(F("Device address: 0x%02x "), finger.device_addr);
    Serial.printf(F("Packet len: %u "), finger.packet_len);
    Serial.printf(F("Baud rate: %u\r\n"), finger.baud_rate);
    Serial.printf(F("Template Count %d\r\n"), finger.getTemplateCount());
#endif // FINGER
//    Preferences key_store;
//    key_store.begin(KEYSTORE_NAME, KEYSTORE_RO);
//    Serial.printf(F("Text Parameter Index %u\r\n"), config.paramIndex);
//    Serial.printf(F("Text Parameter %s\r\n"), config.param);
//    Serial.printf(F("Numeric Parameter %d\r\n"), config.numericParam);
//    for (uint8_t ii = 0; ii < NUM_KEYS; ii++)
//    {
//        char buffer[KEYSTORE_MACRO_MAXLEN * 4];
//        snprintf(buffer, KEYSTORE_LABEL_MAXLEN, "disp%02xlabel", ii);
//        Serial.printf(F("%s: %s\r\n"), buffer, key_store.getString(buffer));
//        snprintf(buffer, KEYSTORE_MACRO_MAXLEN, "disp%02xmacro", ii);
//        Serial.printf(F("%s: %s\r\n"), buffer, key_store.getString(buffer));
//    }
//    key_store.end();
}

void cmd_reset(void)
{
    ESP.restart();
}

//void cmd_input_parameter(void)
//{
//    config.accumParameter = true;
//    config.paramIndex = 0;
//    Serial.println("Enter/Escape");
//    Serial.print("\r\n> ");
//}
//
//void accumulate_parameter(uint8_t ch)
//{
//    switch (ch)
//    {
//        case 13: // Enter
//            config.accumParameter = false;
//            config.param[config.paramIndex] = 0;
//            Serial.print(F("\r\n"));
//            break;
//
//        case 27: // Escape
//            config.accumParameter = false;
//            config.paramIndex = 0;
//            config.param[config.paramIndex] = 0;
//            Serial.print(F("\r\n"));
//            break;
//
//        default:
//            Serial.print((char)ch);
//            if (config.paramIndex < PARAM_LEN)
//            {
//                config.param[config.paramIndex++] = ch;
//            }
//            break;
//    }
//}

//void cmd_set_label(void)
//{
//    if (0 == config.paramIndex)
//    {
//        Serial.println(F("No Parameter"));
//        return;
//    }
//    char buffer[PARAM_LEN];
//    snprintf(buffer, PARAM_LEN, "disp%02xlabel", config.numericParam);
//    Preferences key_store;
//    key_store.begin(KEYSTORE_NAME, KEYSTORE_RW);
//    key_store.putString(buffer, (const char *)config.param);
//    key_store.end();
//}
//
//void cmd_set_macro(void)
//{
//    if (0 == config.paramIndex)
//    {
//        Serial.println(F("No Parameter"));
//        return;
//    }
//    char buffer[PARAM_LEN];
//    snprintf(buffer, PARAM_LEN, "disp%02xmacro", config.numericParam);
//    Preferences key_store;
//    key_store.begin(KEYSTORE_NAME, KEYSTORE_RW);
//    key_store.putString(buffer, (const char *)config.param);
//    key_store.end();
//}

void cmd_clear_keystore(void)
{
//    if (54321 != config.numericParam)
//    {
//        Serial.println(F("Set numeric parameter to 54321 to confirm"));
//        return;
//    }
    Preferences key_store;
    key_store.begin(KEYSTORE_NAME, KEYSTORE_RW);
    key_store.clear();
    key_store_init(key_store);
    key_store.end();
}

void cmd_clear_numeric_parameter(void)
{
//    config.numericParam = 0;
}

void cmd_test_key(void)
{
//    char buffer[40];
//    Serial.printf(F("%s: %s\r\n"), buffer, key_store.getString(buffer));
//    Preferences key_store;
//    key_store.begin(KEYSTORE_NAME, KEYSTORE_RO);
//    key_store.getString(buffer);
//    key_store.end();
}

void cmd_upload_keydefs(void)
{
}

void cmd_serial2_test_string(void)
{
    Serial2.print("abcdefghasdfasd");
}

Command commands_main[] = {
{'?',F("Show Configuration"), [](){ cmd_show_config();}},
{'c',F("Clear Numeric Parameter"), [](){ cmd_clear_numeric_parameter();}},
{'C',F("Clear Keystore"), [](){ cmd_clear_keystore();}},
#ifdef FINGER
{'f',F("Fingerprint Menu"), [](){ cmd_fingerprint_menu(); }},
#endif // FINGER
//{'f',F("Font Select"), [](){ cmd_face_select();}},
{'h',F("Help"), [](){ cmd_help();}},
//{'l',F("Set Label"), [](){ cmd_set_label();}},
//{'m',F("Set Macro"), [](){ cmd_set_macro();}},
//{'p',F("Input Parameter"), [](){ cmd_input_parameter(); }},
{'s',F("Serial2 test string"), [](){ cmd_serial2_test_string(); }},
{'t',F("Test key (parameter)"), [](){ cmd_test_key(); }},
{'u',F("Upload key definitions"), [](){ cmd_upload_keydefs(); }},
{' ',F("Reset"), [](){ cmd_reset(); }},
{'&',F("Reset"), [](){ cmd_reset(); }}
};

#ifdef FINGER

void fingerprint_error(int8_t id)
{
    switch(id)
    {
    case FINGERPRINT_OK:
        Serial.println(F("FINGERPRINT_OK")); break;
    case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println(F("FINGERPRINT_PACKETRECIEVEERR")); break;
    case FINGERPRINT_NOFINGER:
        Serial.println(F("FINGERPRINT_NOFINGER")); break;
    case FINGERPRINT_IMAGEFAIL:
        Serial.println(F("FINGERPRINT_IMAGEFAIL")); break;
    case FINGERPRINT_IMAGEMESS:
        Serial.println(F("FINGERPRINT_IMAGEMESS")); break;
    case FINGERPRINT_FEATUREFAI:
        Serial.println(F("FINGERPRINT_FEATUREFAI")); break;
    case FINGERPRINT_NOMATCH:
        Serial.println(F("FINGERPRINT_NOMATCH")); break;
    case FINGERPRINT_NOTFOUND:
        Serial.println(F("FINGERPRINT_NOTFOUND")); break;
    case FINGERPRINT_ENROLLMISMATCH:
        Serial.println(F("FINGERPRINT_ENROLLMISMATCH")); break;
    case FINGERPRINT_BADLOCATION:
        Serial.println(F("FINGERPRINT_BADLOCATION")); break;
    case FINGERPRINT_DBRANGEFAIL:
        Serial.println(F("FINGERPRINT_DBRANGEFAIL")); break;
    case FINGERPRINT_UPLOADFEATUREFAIL:
        Serial.println(F("FINGERPRINT_UPLOADFEATUREFAIL")); break;
    case FINGERPRINT_PACKETRESPONSEFAIL:
        Serial.println(F("FINGERPRINT_PACKETRESPONSEFAIL")); break;
    case FINGERPRINT_UPLOADFAIL:
        Serial.println(F("FINGERPRINT_UPLOADFAIL")); break;
    case FINGERPRINT_DELETEFAIL:
        Serial.println(F("FINGERPRINT_DELETEFAIL")); break;
    case FINGERPRINT_DBCLEARFAIL:
        Serial.println(F("FINGERPRINT_DBCLEARFAIL")); break;
    case FINGERPRINT_PASSFAIL:
        Serial.println(F("FINGERPRINT_PASSFAIL")); break;
    case FINGERPRINT_INVALIDIMAGE                                               \:
        Serial.println(F("FINGERPRINT_INVALIDIMAGE                                               \")); break;
    case FINGERPRINT_FLASHER:
        Serial.println(F("FINGERPRINT_FLASHER")); break;
    case FINGERPRINT_INVALIDREG:
        Serial.println(F("FINGERPRINT_INVALIDREG")); break;
    case FINGERPRINT_ADDRCODE:
        Serial.println(F("FINGERPRINT_ADDRCODE")); break;
    default:
        Serial.printf(F("Unknown error %d\r\n"), id); break;
    }
}

void cmd_fingerprint_menu(void)
{
    command_set = commands_fingerprint;
}

void cmd_fingerprint_init()
{
  // set the data rate for the sensor serial port
  Serial2.begin(57600);
  finger.begin(57600);

  if (finger.verifyPassword()) 
  {
      Serial.println(F("Found fingerprint sensor"));
  }
  else
  {
    while (1)
    {
        Serial.println(F("No fingerprint sensor"));
        return;
    }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
}

void cmd_fingerprint_ledtest(void)
{
    Serial.println(F("cmd_fingerprint_ledtest begins"));
    finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
    delay(250);
    finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_BLUE);
    delay(250);
    finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_PURPLE);
    delay(250);

    // flash red LED
    finger.LEDcontrol(FINGERPRINT_LED_FLASHING, 25, FINGERPRINT_LED_RED, 10);
    delay(2000);
    // Breathe blue LED till we say to stop
    finger.LEDcontrol(FINGERPRINT_LED_BREATHING, 100, FINGERPRINT_LED_BLUE);
    delay(3000);
    finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_ON, 200, FINGERPRINT_LED_PURPLE);
    delay(2000);
    finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_OFF, 200, FINGERPRINT_LED_PURPLE);
    delay(2000);
    Serial.println(F("cmd_fingerprint_ledtest ends"));
}

void cmd_fingerprint_delete(int32_t id)
{
    Serial.printf(F("Deleting %d\r\n"), id);
    uint8_t p = finger.deleteModel(id);
    fingerprint_error(p);
}

void cmd_fingerprint_clear()
{
    finger.emptyDatabase();
}

void cmd_fingerprint_enroll(int32_t id)
{
    int p = -1;
    Serial.printf(F("Waiting for valid finger to enroll as #%u\r\n"), id);
    while (p != FINGERPRINT_OK) {
        p = finger.getImage();
        fingerprint_error(p);
    }

    // OK success!

    p = finger.image2Tz(1);
    fingerprint_error(p);
    if (FINGERPRINT_OK != p)
    {
        return;
    }

    Serial.println(F("Remove finger"));
    delay(2000);
    p = 0;
    while (p != FINGERPRINT_NOFINGER) 
    {
        p = finger.getImage();
    }
    Serial.printf(F("ID %u\r\n"), id);
    p = -1;
    Serial.println("Place same finger again");
    while (p != FINGERPRINT_OK) 
    {
        p = finger.getImage();
        fingerprint_error(p);
    }

    p = finger.image2Tz(2);
    fingerprint_error(p);
    // OK converted!
    Serial.printf(F("Creating model for id %d\r\n"), id);

    p = finger.createModel();
    fingerprint_error(p);

    p = finger.storeModel(id);
    fingerprint_error(p);
    if (FINGERPRINT_OK != p)
    {
        return;
    }
}

void cmd_fingerprint_get_id()
{
    uint8_t p = finger.getImage();
    fingerprint_error(p);
    // OK success!

    p = finger.image2Tz();
    fingerprint_error(p);
    if (FINGERPRINT_OK != p)
    {
        return;
    }

    // OK converted!
    p = finger.fingerSearch();
    fingerprint_error(p);
    // found a match!
    Serial.printf(F("Found ID %d with confidence %d\r\n"), finger.fingerID, finger.confidence);
}

Command commands_fingerprint[] = {
{'c',F("Clear Numeric Parameter"), [](){ cmd_clear_numeric_parameter(); }},
{'C',F("Clear Fingerprint DB"), [](){ cmd_fingerprint_clear(); }},
{'e',F("Enroll new fingerprint to id (parameter)"), [](){ cmd_fingerprint_enroll(numericParameter); }},
{'h',F("Help"), [](){ cmd_help(); }},
{'i',F("Init"), []() { cmd_fingerprint_init(); }},
{'l',F("LED Test"), []() { cmd_fingerprint_ledtest(); }},
{'r',F("Read Fingerprint"), []() { cmd_fingerprint_get_id(); }},
{'x',F("Return to main menu"), []() { command_set = commands_main; }},
{'&',F("Reset"), [](){ cmd_reset(); }}
};

#endif // FINGER

void handleCommand(uint8_t command)
{
//    if (config.accumParameter == true)
//    {
//        accumulate_parameter(command);
//        return;
//    }
    for (Command * iter = command_set; iter->key != '&'; iter++)
    {
        if (iter->key == command)
        {
            iter->fun();
            return;
        }
    }
    if (command >= '0' & command <= '9')
    {
        numericParameter *= 10;
        numericParameter += command - '0';
    }
}

void key_store_init(Preferences ks)
{
    Serial.println(F("key_store_init"));
    ks.begin(KEYSTORE_NAME, KEYSTORE_RW);
    ks.putBool("init", true);

    for (uint8_t ii = 0; ii < NUM_KEYS; ii++)
    {
        char buffer[20];
        sprintf(buffer, "disp%02xlabel", ii);
        ks.putString(buffer, buffer);
        sprintf(buffer, "disp%02xmacro", ii);
        ks.putString(buffer, buffer);
    }
    Serial.println(F("key_store_init done"));
}


void setup(void)
{
//    memset(&config, 0, sizeof(config));
    command_set = commands_main;
    Serial.begin(115200);
    delay(2000);
    Serial.println("hello");
#ifdef DISPLAY
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        for (;;)
        {
            Serial.println(F("SSD1306 allocation failed"));
            delay(1000);
        }
    }
    redisplay();
#endif // DISPLAY
//    Preferences key_store;
//    key_store.begin(KEYSTORE_NAME, KEYSTORE_RW);
//    if (false == key_store.isKey("init"))
//    {
//        key_store_init(key_store);
//    }
//    key_store.end();
}


void loop(void)
{
    delay(500);
    if (Serial.available())
    {
        handleCommand(Serial.read());
    }
}
