#include <Arduino.h>
//#include <YetAnotherPcInt.h>
//#include <SoftwareSerial.h>
#include <stdlib.h>
#include <Wire.h>
#include "hardware/watchdog.h"
#include <ansi.h>
#include "line_editor.h"
#define OLED
#ifdef OLED
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans12pt7b.h>
#endif // OLED
#include <Keyboard.h>
#include <Mouse.h>
#define FINGERPRINT
#ifdef FINGERPRINT
#include <Adafruit_Fingerprint.h>
#endif // FINGERPRINT
#include <AsyncTimer.h>

#define KV
#ifdef KV
#include "keyvalue.h"
KeyValue kv;
#endif // KV

AsyncTimer at;
ANSI ansi(&Serial);

// Fingerprint Sensor
// 1 White VTouch
// 2 Blue Touchout
// 3 Green vcc 3v3
// 4 Yellow rx <-> controller rx
// 5 Black tx <-> controller tx
// 6 Red gnd

#define SCREEN_WIDTH 128 // byte display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#ifdef OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);
#endif // OLED

const uint8_t PIN_TOUCH = 2;
const uint8_t PIN_FINGERPRINT_RX = 5;
const uint8_t PIN_FINGERPRINT_TX = 4;
const uint8_t PIN_FINGERPRINT_POWER = 15;

const uint8_t PIN_OLED_SDA = 26;
const uint8_t PIN_OLED_SCL = 27;
const uint16_t JIGGLE_TIMEOUT = 5000;

#define FSH(s) reinterpret_cast<const __FlashStringHelper *>(s)
typedef struct
{
    char key;
    const __FlashStringHelper * help;
    void (*fun)();
}Command;

typedef enum
{
    INPUT_COMMAND,
    INPUT_KEY,
    INPUT_VALUE,
    INPUT_READY
} InputState;

//#define PARAM_LEN 32
struct {
    InputState inputState;
    String param1;
    String param2;
    char storedCommand;
    int32_t numericParam;
    int8_t selection;
    bool powered_on;
    bool jiggler_on;
    bool pressed;
    bool released;
    bool pressed_handled;
    bool longpress_handled;
    int32_t last_touched;
    int oled_timeout;
    bool accum_numbers;
    String sending;
    int8_t sending_index;
}config;

const int EDITOR_BUFFER_LEN = 32;
char editor_buffer[EDITOR_BUFFER_LEN] = "i am a buffer";


#ifdef FINGERPRINT
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);
#endif // FINGERPRINT

void cmd_clear_numeric_parameter(void);
void setup(void);

#ifdef FINGERPRINT
void fingerprint_error(int8_t);
void cmd_fingerprint_init();
void cmd_fingerprint_delete(int32_t);
void cmd_fingerprint_enroll(uint8_t);
#endif // FINGERPRINT
void loop(void);

// --------------------------------------------------------------------------
// OLED Routines
// --------------------------------------------------------------------------
void oled_init();

void oled_init()
{
#ifdef OLED
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
    {
        Serial.println(F("SSD1306 allocation failed"));
        return;
    }
    Serial.println(F("SSD1306 Init Ok"));
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0, SCREEN_HEIGHT-1);             // Start at top-left corner
    display.setFont(&FreeSans12pt7b);
    display.display();
    if (-1 == config.oled_timeout)
    {
        config.oled_timeout = at.setTimeout(oled_init, 3000);
    }
    else
    {
        at.reset(config.oled_timeout);
    }
//    oled_redisplay();
#endif // OLED
}

void oled_print(String & text)
{
    oled_print(text.c_str());
}

void oled_print(const char * text)
{
    display.clearDisplay();
    display.setCursor(0, SCREEN_HEIGHT-8);             // Start at top-left corner
    display.println(text);
    display.display();
}
// void cmd_oled_demo()
// {
//     oled_init();
//     for (uint8_t ii = 0; ii < 5; ii++)
//     {
//         display.clearDisplay();
//         display.setCursor(0, SCREEN_HEIGHT-8);             // Start at top-left corner
//         display.println(F("Hello"));
//         display.display();
//         delay(500);
//         display.clearDisplay();
//         display.setCursor(0, SCREEN_HEIGHT-8);             // Start at top-left corner
//         display.println(F("Goodbye"));
//         display.display();
//         delay(500);
//     }
//     display.clearDisplay();
//     display.display();
// }

void cmd_clear_numeric_parameter(void)
{
    config.numericParam = 0;
}

void touched() 
{
    config.pressed = digitalRead(PIN_TOUCH);
    config.released = !config.pressed;
    if (config.pressed)
    {
        config.last_touched = millis();
    }
}

#ifdef FINGERPRINT
void fingerprint_error(int8_t id)
{
    switch(id)
    {
//    case FINGERPRINT_OK:
//        Serial.println(F("FINGERPRINT_OK")); break;
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
        break;
    }
}

void cmd_fingerprint_init()
{
    Serial.println(F("cmd_fingerprint_init"));

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
}

void cmd_fingerprint_delete(int32_t id)
{
    byte p = finger.deleteModel(id);
    fingerprint_error(p);
}

void cmd_fingerprint_clear()
{
    finger.emptyDatabase();
}

void cmd_fingerprint_enroll(uint8_t id)
{
    int p = -1;
    Serial.println(F("Waiting for valid finger"));
    while (p != FINGERPRINT_OK) {
        p = finger.getImage();
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
    p = -1;
    Serial.println(F("Place same finger again"));
    while (p != FINGERPRINT_OK) 
    {
        p = finger.getImage();
    }

    p = finger.image2Tz(2);
    fingerprint_error(p);
    // OK converted!
    Serial.println(F("Creating model"));

    p = finger.createModel();
    fingerprint_error(p);

    p = finger.storeModel(id);
    fingerprint_error(p);
}

int8_t cmd_fingerprint_get_id()
{
    byte p = finger.getImage();
    fingerprint_error(p);
    if (FINGERPRINT_OK != p)
    {
        return -1;
    }
    // OK success!

    p = finger.image2Tz();
    fingerprint_error(p);
    if (FINGERPRINT_OK != p)
    {
        return -1;
    }

    // OK converted!
    p = finger.fingerSearch();
    fingerprint_error(p);
    if (FINGERPRINT_OK != p)
    {
        return -1;
    }
    // found a match!
    Serial.print(F("Found ID "));
    Serial.print(finger.fingerID);
    Serial.print(F(" confidence "));
    Serial.print(finger.confidence);
    return finger.fingerID;
}
#endif // FINGERPRINT

void power_on()
{
    if (true == config.powered_on)
    {
        return;
    }
    config.powered_on = true;
    digitalWrite(PIN_FINGERPRINT_POWER, 1);
}

void power_off()
{
    if (false == config.powered_on)
    {
        return;
    }
    config.powered_on = false;
    digitalWrite(PIN_FINGERPRINT_POWER, 0); 
}

bool accum_parameter(char command)
{
    Serial.print(command);
    if ('\r' == command)
    {
        return 0 == config.param1.length() ? false : true;
    }
    if ('\x1b' == command)
    {
        config.inputState = INPUT_COMMAND;
        return false;
    }
    if (config.accum_numbers && !isDigit(command))
    {
        return false;
    }
    config.param1 += command;
    return false;
}

void handle_input(char command)
{
    switch (config.inputState)
    {
    case INPUT_KEY:
        if (true == accum_parameter(command))
        {
            config.inputState = INPUT_VALUE;
//            Serial.printf("Key is %s\r\n", config.param1.c_str());
            config.param2 = config.param1;
            config.param1 = "";
            Serial.print(F("\r\nValue: (Enter to finish): "));
        }
        break;

    case INPUT_VALUE:
        if (true == accum_parameter(command))
        {
//            Serial.printf("Value is %s\r\n", config.param1.c_str());
            Serial.println("");
            config.inputState = INPUT_READY;
//            config.paramIndex = 0;
            handle_command(config.storedCommand);
        }
        break;

    case INPUT_COMMAND:
        handle_command(command);
        break;
    }
}

void handle_kv_delete(byte command)
{
    if (INPUT_READY == config.inputState)
    {
        config.accum_numbers = false;
        kv.removeIndex(config.param1.toInt());
        kv.save();
        config.inputState = INPUT_COMMAND;
        return;
    }
    Serial.print(F("\r\nDelete: (Enter to finish): "));
    config.param1 = "";
    config.param2 = "";
    config.inputState = INPUT_VALUE;
    config.accum_numbers = true;
    config.storedCommand = command;
}

void jiggle0()
{
    Serial.println(F("jiggle0"));
    if (!config.jiggler_on)
    {
        return;
    }
    at.setTimeout(jiggle1, JIGGLE_TIMEOUT);
    Mouse.begin();
    Mouse.move(10, 0);
    Mouse.end();
}

void jiggle1()
{
    Serial.println(F("jiggle1"));
    if (!config.jiggler_on)
    {
        return;
    }
    at.setTimeout(jiggle0, JIGGLE_TIMEOUT);
    Mouse.begin();
    Mouse.move(-10, 0);
    Mouse.end();
}

void handle_command(byte command)
{
    String key;
    String value;
    byte count;
#ifdef KV
    Node * node;
#endif // KV

    switch (command)
    {
        case 'a': power_on(); break;
        case 'A': power_off(); break;
#ifdef FINGERPRINT
        case 'd': handle_kv_delete(command); break;
        case 'e': cmd_fingerprint_enroll(0); break;
        case 'f': cmd_fingerprint_init(); break;
        case 'g': cmd_fingerprint_get_id(); break;
#endif // FINGERPRINT
#ifdef KV
        case 'k': 
            for (count = 0; count < kv.count(); count++)
            {
                Serial.printf("%d: %s = %s\r\n",
                     count, kv.key(count).c_str(), kv.value(count).c_str());
            }
            Serial.println(F("Done"));
            
            break;

        case 'l': kv.load(); break;
        case 'p': 
            if (INPUT_READY == config.inputState)
            {
                kv.put(config.param2.c_str(), config.param1.c_str());
                kv.save();
                config.inputState = INPUT_COMMAND;
                return;
            }
            Serial.print(F("\r\nKey: (Enter to finish): "));
            config.param1 = "";
            config.param2 = "";
            config.inputState = INPUT_KEY;
            config.storedCommand = command;
            break;

        case 'r': while (1); break;

        case 's':
            kv.save();
            break;

        case 'x':
            line_editor(ansi, editor_buffer, EDITOR_BUFFER_LEN);
            break;

#endif // KV
        default:
            Serial.println(F("a = power on"));
            Serial.println(F("A = power off"));
            Serial.println(F("d = delete kv"));
            Serial.println(F("e = enroll"));
            Serial.println(F("f = fingeprint init"));
            Serial.println(F("g = get fingerprint id"));
            Serial.println(F("k = list keys"));
            Serial.println(F("l = load KV"));
            Serial.println(F("p = put key/value"));
            Serial.println(F("r = reset"));
            Serial.println(F("s = save KV"));
            Serial.println(F("x = line editor test"));
            break;
    }
}

void setup(void)
{
    memset(&config, 0, sizeof(config));
    Serial.begin(115200);

    Serial.println("Setup");
#ifdef FINGERPRINT 
    Serial1.begin(57600);
#endif // FINGERPRINT
//    if (watchdog_caused_reboot())
//    {
//        while (!Serial.available())
//        {
//            Serial.println(
//                F("Rebooted by watchdog, press to continue in safe mode"));
//            delay(5000);
//        }
//        return;
//    }
    pinMode(PIN_FINGERPRINT_POWER, OUTPUT);
    Wire1.setSDA(PIN_OLED_SDA);
    Wire1.setSCL(PIN_OLED_SCL);
    Wire1.begin();
    Wire.begin();

    pinMode(PIN_TOUCH, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_TOUCH), touched, CHANGE);
    config.oled_timeout = -1;
    config.powered_on = true;
    power_off();
    oled_init();
    display.clearDisplay();
    display.display();
    kv.load();
//    watchdog_enable(1000, 1);
}

void keyboard_send()
{
    if (0 == config.sending_index)
    {
        Keyboard.begin();
    }
    if (config.sending_index < config.sending.length())
    {
        Keyboard.write(config.sending[config.sending_index++]);
        at.setTimeout(keyboard_send, 20);
    }
}

void handle_longpress()
{
    power_on();
    oled_init();
    delay(300);
    cmd_fingerprint_init();
    int8_t results = cmd_fingerprint_get_id();
    Serial.printf("handle_longpress result %d\r\n", results);
    if (results > -1)
    {
        if (config.selection == kv.count())
        {
            config.jiggler_on = !config.jiggler_on;
            Serial.printf("Good fingerprint, Jiggler %s\r\n",
               config.jiggler_on ? "on" : "off");

            if (config.jiggler_on)
            {
                at.setTimeout(jiggle0, JIGGLE_TIMEOUT);
            }
        }
        else
        {
            Serial.printf("Good fingerprint, selection %s\r\n",
                 kv.key(config.selection).c_str());
            config.sending = kv.value(config.selection);
            config.sending_index = 0;
            keyboard_send();
        }
    }
    at.setTimeout(power_off, 500);
//    at.setTimeout(oled_init, 1000);
}

void update_display()
{
//    Serial.printf("touched %s\r\n", kv.key(config.selection).c_str());
    int8_t count = kv.count();
    if (count == config.selection)
    {
        if (true == config.jiggler_on)
        {
            oled_print("Jiggler Off");
        }
        else
        {
            oled_print("Jiggler On");
        }
    }
    else
    {
        oled_print(kv.key(config.selection));
    }
}

void loop(void)
{
    at.handle();
    watchdog_update();

    if (config.pressed && (500 < (millis() - config.last_touched)))
    {
        Serial.printf("long press, selection %d\r\n", config.selection);
        handle_longpress();
        at.setTimeout(
            [](){ attachInterrupt(digitalPinToInterrupt(PIN_TOUCH), touched,
                    CHANGE); }, 2000);
        detachInterrupt(digitalPinToInterrupt(PIN_TOUCH));
        config.pressed = false;
    }

    if (config.released && (500 > (millis() - config.last_touched)))
    {
        config.selection++;
        config.selection %= 1 + kv.count();
        update_display();
        config.released = false;
    }
    if (Serial.available())
    {
        handle_input(Serial.read());
    }
}

