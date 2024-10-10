#include <stdint.h>
#include <Wire.h>
#include "muxmux.h"
//#include <oled.h>
//#include <Preferences.h>
//#include <Keyboard.h>
//#include <ArduinoYaml.h>
#ifdef FINGER
#include <Adafruit_Fingerprint.h>
#endif // FINGER

//#define DFORMAT_SPIFFS_IF_FAILED true
// #ifdef DFORMAT_SPIFFS_IF_FAILED
// #error BORK
// #endif 
#define OLED_DISPLAY
#ifdef OLED_DISPLAY
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
//#include <FreeSans12pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
//#include <Adafruit_GFX/Fonts/FreeSans12pt7b.h>
#endif // OLED_DISPLAY

// #include <Effortless_SPIFFS.h>
#include <string>

#define NUM_KEYS 8
#define KEYSTORE_NAME "kv"

#define KEYSTORE_RW false
#define KEYSTORE_RO true

#define KEYSTORE_LABEL_MAXLEN 32
#define KEYSTORE_MACRO_MAXLEN 64
#define PARAM_LEN KEYSTORE_MACRO_MAXLEN * 4

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

#define PIN_RESET 9

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



struct {
//    uint8_t fontIndex;
//    uint8_t size;
    uint8_t param[PARAM_LEN];
    uint8_t paramIndex;
    int32_t numericParam;
    bool accumParameter;
}config;
int32_t numericParameter;
#ifdef FINGER
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);
#endif // FINGER

// eSPIFFS fileSystem(&Serial);
#ifdef OLED_DISPLAY
MuxMux mux(0);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &mux, OLED_RESET);
#endif // OLED_DISPLAY
//Keyboard kb();
Command * command_set;
extern Command commands_main[];
extern Command commands_fingerprint[];
extern Command commands_i2c[];

// FontSel fonts[] = {
//     { F("Sans9pt7b"), &FreeSans9pt7b },
//     { F("Sans12pt7b"), &FreeSans12pt7b }
// };
// 
// #define NUM_FONTS (sizeof(fonts)/sizeof(fonts[0]))

void handleCommand(uint8_t command);
void cmd_help();
void redisplay();
void cmd_reset(void);
void cmd_clear_keystore(void);
void cmd_clear_numeric_parameter(void);
void cmd_test_key(void);
void cmd_upload_keydefs(void);
void cmd_show_config(void);
//void key_store_init(Preferences ks);
// void cmd_serial2_test_string(void);
// void cmd_receive_yaml(void);
void setup(void);
#ifdef FINGER
void fingerprint_error(int8_t);
void cmd_fingerprint_init();
void cmd_fingerprint_menu(void);
void cmd_fingerprint_ledtest(void);
void cmd_fingerprint_ledtest2(void);
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
#ifdef OLED_DISPLAY
    mux.setPort(0);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0, SCREEN_HEIGHT-1);             // Start at top-left corner
    display.setFont(&FreeSans12pt7b);
    display.display();
#endif // OLED_DISPLAY
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
    Serial.println(F("Reading sensor parameters"));
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
//    Preferences key_store;
//    key_store.begin(KEYSTORE_NAME, KEYSTORE_RW);
//    key_store.clear();
//    key_store_init(key_store);
//    key_store.end();
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

// void cmd_upload_keydefs(void)
// {
// }
// 
// void cmd_serial2_test_string(void)
// {
//     Serial2.print("abcdefghasdfasd");
// }

//bool xmodem_receive_block(void *blk_id, size_t idSize, byte *data, size_t dataSize) 
//{
//  byte id = *((byte *) blk_id);
//  for(int i = 0; i < dataSize; ++i) {
//    //do stuff with the recieved data
//  }
//  //return false to stop the transfer early
//  return false;
//}

void cmd_upload_keydefs(void)
{
    char end_string[16];
    sprintf(end_string, "%d", numericParameter);
    Serial.println(F("Keydef Upload Start"));
    Serial.println(F("Keydef Upload End"));
}

void cmd_i2c_scan(void)
{
    i2c_scanner();
}

// void cmd_spiffs_test(void)
// {
//     fileSystem.checkFlashConfig();
//     
// //    if (!fileSystem.checkFlashConfig()) {
// //      Serial.println("Flash size was not correct! Please check your SPIFFS config and try again");
// //    }
// }

void cmd_oled_init()
{
    Serial.println(F("cmd_oled_init"));
    mux.setPort(0);
#ifdef OLED_DISPLAY
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        for (;;)
        {
            Serial.println(F("SSD1306 allocation failed"));
            delay(1000);
        }
    }
    redisplay();
#endif // OLED_DISPLAY
}

Command commands_main[] = {
{'?',F("Show Configuration"), [](){ cmd_show_config();}},
{'c',F("Clear Numeric Parameter"), [](){ cmd_clear_numeric_parameter();}},
//{'C',F("Clear Keystore"), [](){ cmd_clear_keystore();}},
#ifdef FINGER
{'f',F("Fingerprint Menu"), [](){ cmd_fingerprint_menu(); }},
#endif // FINGER
//{'f',F("Font Select"), [](){ cmd_face_select();}},
{'h',F("Help"), [](){ cmd_help();}},
{'i',F("I2C Menu"), [](){ command_set = commands_i2c;}},
//{'l',F("Set Label"), [](){ cmd_set_label();}},
//{'m',F("Set Macro"), [](){ cmd_set_macro();}},
{'o',F("OLED Init"), [](){ cmd_oled_init(); }},
//{'p',F("Input Parameter"), [](){ cmd_input_parameter(); }},
//{'s',F("Serial2 test string"), [](){ cmd_serial2_test_string(); }},
//{'s',F("SPIFFS Test"), [](){ cmd_spiffs_test(); }},
//{'t',F("Test key (parameter)"), [](){ cmd_test_key(); }},
//{'u',F("Upload key definitions"), [](){ cmd_upload_keydefs(); }},
//{'y',F("Upload YAML File"), [](){ cmd_upload_yaml(); }},
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
    case FINGERPRINT_FEATUREFAIL:
        Serial.println(F("FINGERPRINT_FEATUREFAIL")); break;
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
    case FINGERPRINT_INVALIDIMAGE:
        Serial.println(F("FINGERPRINT_INVALIDIMAGE")); break;
    case FINGERPRINT_FLASHERR:
        Serial.println(F("FINGERPRINT_FLASHERR")); break;
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
    for (uint8_t ii = 0; ii < 10; ii++)
    {
        finger.LEDcontrol(true);
        delay(100);
        finger.LEDcontrol(false);
        delay(100);
    }
}

void cmd_fingerprint_ledtest2(void)
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

void cmd_show_i2c_config()
{
    Serial.println(F("I2C Config"));
//    Serial.printf(F("Current Port: %d\r\n"), mux.getPort());
}

Command commands_i2c[] = {
{'?',F("Show Configuration"), [](){ cmd_show_i2c_config();}},
//{'c',F("Clear Numeric Parameter"), [](){ cmd_clear_numeric_parameter();}},
//{'C',F("Clear Keystore"), [](){ cmd_clear_keystore();}},
//{'f',F("Font Select"), [](){ cmd_face_select();}},
{'h',F("Help"), [](){ cmd_help();}},
//{'i',F("I2C Scan"), [](){ cmd_i2c_scan();}},
//{'l',F("Set Label"), [](){ cmd_set_label();}},
//{'m',F("Set Macro"), [](){ cmd_set_macro();}},
{'p',F("Port Select"), [](){ mux.setPort(config.numericParam); }},
//{'s',F("Serial2 test string"), [](){ cmd_serial2_test_string(); }},
{'s',F("Device Scan"), [](){ cmd_i2c_scan(); }},
//{'t',F("Test key (parameter)"), [](){ cmd_test_key(); }},
//{'u',F("Upload key definitions"), [](){ cmd_upload_keydefs(); }},
//{'y',F("Upload YAML File"), [](){ cmd_upload_yaml(); }},
{'x',F("Back to main"), [](){ command_set = commands_main; }},
{'&',F("Reset"), [](){ cmd_reset(); }}
};

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

// void key_store_init(Preferences ks)
// {
//     Serial.println(F("key_store_init"));
//     ks.begin(KEYSTORE_NAME, KEYSTORE_RW);
//     ks.putBool("init", true);
// 
//     for (uint8_t ii = 0; ii < NUM_KEYS; ii++)
//     {
//         char buffer[20];
//         sprintf(buffer, "disp%02xlabel", ii);
//         ks.putString(buffer, buffer);
//         sprintf(buffer, "disp%02xmacro", ii);
//         ks.putString(buffer, buffer);
//     }
//     Serial.println(F("key_store_init done"));
// }


void setup(void)
{
    pinMode(PIN_RESET, INPUT_PULLUP);
//    digitalWrite(PIN_RESET, HIGH);
    memset(&config, 0, sizeof(config));
    command_set = commands_main;
    Serial.begin(115200);
    i2c_scanner_setup();
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
    if (Serial.available())
    {
        handleCommand(Serial.read());
    }
}
