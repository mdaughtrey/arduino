#include <Arduino.h>
//#include <YetAnotherPcInt.h>
//#include <SoftwareSerial.h>
#include <stdint.h>
#include <Wire.h>
#define OLED
#ifdef OLED
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans12pt7b.h>
#endif // OLED
//#include <EEPROM.h>
//#include <Preferences.h>
//#include <Keyboard.h>
#define FINGERPRINT
#ifdef FINGERPRINT
#include <Adafruit_Fingerprint.h>
#endif // FINGERPRINT
#include <AsyncTimer.h>

AsyncTimer at;


// Fingerprint Sensor
// 1 White VTouch
// 2 Blue Touchout
// 3 Green vcc 3v3
// 4 Yellow rx <-> controller rx
// 5 Black tx <-> controller tx
// 6 Red gnd

#define SCREEN_WIDTH 128 // OLED display width, in pixels
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

#define FSH(s) reinterpret_cast<const __FlashStringHelper *>(s)
typedef struct
{
    char key;
    const __FlashStringHelper * help;
    void (*fun)();
}Command;

struct {
//    byte param[PARAM_LEN];
//    byte paramIndex;
    int32_t numericParam;
    bool finger_touch;
//    bool accumParameter;
    uint8_t numClicks;
}config;


#ifdef FINGERPRINT
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);
#endif // FINGERPRINT

void cmd_help();
void cmd_reset(void);
void cmd_clear_numeric_parameter(void);
void cmd_test_key(void);
void cmd_show_config(void);
void setup(void);

#ifdef FINGERPRINT
void fingerprint_error(int8_t);
void cmd_fingerprint_init();
void cmd_fingerprint_menu(void);
void cmd_fingerprint_ledtest(void);
void cmd_fingerprint_ledtest2(void);
void cmd_fingerprint_delete(int32_t);
void cmd_fingerprint_enroll(uint8_t);
#endif // FINGERPRINT
void loop(void);

// --------------------------------------------------------------------------
// OLED Routines
// --------------------------------------------------------------------------
void cmd_oled_init();

void cmd_oled_init()
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
//    oled_redisplay();
#endif // OLED
}

void cmd_oled_demo()
{
    cmd_oled_init();
    for (uint8_t ii = 0; ii < 5; ii++)
    {
        display.clearDisplay();
        display.setCursor(0, SCREEN_HEIGHT-8);             // Start at top-left corner
        display.println(F("Hello"));
        display.display();
        delay(500);
        display.clearDisplay();
        display.setCursor(0, SCREEN_HEIGHT-8);             // Start at top-left corner
        display.println(F("Goodbye"));
        display.display();
        delay(500);
    }
    display.clearDisplay();
    display.display();
}

void cmd_show_config(void)
{
//    Serial.printf(F("parameter %d\r\n"), config.numericParam);
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
//    for (byte ii = 0; ii < NUM_KEYS; ii++)
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
}


void cmd_clear_numeric_parameter(void)
{
    config.numericParam = 0;
}

void touched() // void * ptr, bool pinstate)
{
    config.finger_touch = true;
}

#ifdef FINGERPRINT
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
        break;
//        Serial.printf(F("Unknown error %d\r\n"), id); break;
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

//  Serial.println(F("Reading sensor parameters"));
//  finger.getParameters();
//  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
//  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
//  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
//  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
//  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
//  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
//  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
}

void cmd_fingerprint_ledtest(void)
{
    for (byte ii = 0; ii < 10; ii++)
    {
        finger.LEDcontrol(true);
        delay(1000);
        finger.LEDcontrol(false);
        delay(1000);
    }
}

void cmd_fingerprint_ledtest2(void)
{
    finger.LEDcontrol(FINGERPRINT_LED_BREATHING,
        128,
        10,
        100);
//uint8_t Adafruit_Fingerprint::LEDcontrol(uint8_t control, uint8_t speed,
//                                         uint8_t coloridx, uint8_t count) {
//  SEND_CMD_PACKET(FINGERPRINT_AURALEDCONFIG, control, speed, coloridx, count);
} 

void cmd_fingerprint_delete(int32_t id)
{
//    Serial.printf(F("Deleting %d\r\n"), id);
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
//        fingerprint_error(p);
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
//    Serial.printf(F("ID %u\r\n"), id);
    p = -1;
    Serial.println("Place same finger again");
    while (p != FINGERPRINT_OK) 
    {
        p = finger.getImage();
 //       fingerprint_error(p);
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

#ifdef EEPROM
void cmd_keyboard_write()
{
    byte index = 0;
    Serial.println("write");
//    Keyboard.begin();
//    Keyboard.releaseAll();
    while (1)
    {
        char cc = EEPROM.read(index++);
        if (cc == '\0')
            break;
        Serial.print(cc);
//        Keyboard.write(cc);
        delay(100);
    }
//    Keyboard.end();
}

void cmd_record_password()
{
    byte index = 0;
    Serial.println("record");
    while (1)
    {
        while (!Serial.available())
            delay(50);
        char cc = Serial.read();
        Serial.print(cc);
        EEPROM.write(index++, cc == 0x1b ? '\0': cc);
        if (cc == 0x1b)
        {
            Serial.println("Done recording");
            break;
        }
    }
}
#endif // EEPROM

#ifdef FINGERPRINT
void cmd_led_demo()
{
  // LED fully on
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
}

#endif // FINGERPRINT

void cmd_poll()
{
    for (uint8_t ii = 0; ii < 20; ii++)
    {
        Serial.println(digitalRead(PIN_TOUCH));
        delay(500);
    }
}

void cmd_fingerprint_touched()
{
#ifdef FINGERPRINT
    digitalWrite(PIN_FINGERPRINT_POWER, 1);
    delay(200);
    int8_t results = cmd_fingerprint_get_id();
    Serial.print(F("cmd_fingerprint_get_id "));
    Serial.println((int)results);
    at.setTimeout([](){ digitalWrite(PIN_FINGERPRINT_POWER, 0); }, 500);
    if (results > -1)
    {
        Serial.println(F("Run demo"));
        cmd_oled_demo();
    }
#endif // FINGERPRINT
}

void power_on()
{
    digitalWrite(PIN_FINGERPRINT_POWER, 0);
    display.clearDisplay();
    display.display();
}

void power_off()
{
    display.clearDisplay();
    display.display();
    digitalWrite(PIN_FINGERPRINT_POWER, 1); 
}

void handleCommand(byte command)
{
    switch (command)
    {
//        case 'i': 
//            i2c_scanner();
//            break;
        case 'o': cmd_poll(); break;
#ifdef OLED
        case 'd': cmd_oled_demo(); break;
#endif // OLEd
#ifdef FINGERPRINT
        case 'c': fingerprint_error(finger.createModel()); break;
        case 'e': cmd_fingerprint_enroll(0); break;
        case 'C': cmd_show_config(); break;
        case 'g': cmd_fingerprint_get_id(); break;
        case 'i': cmd_fingerprint_init(); break;
        case 'p': power_off(); break;
        case 'P': power_on(); break;
            break;
//        case 'r': oled_redisplay(); break;
        case 't': cmd_fingerprint_touched(); break;
        case 'T': Serial.println(finger.getTemplateCount()); break;
        case 'f': Serial.println(finger.fingerFastSearch()); break;
        
//        case 'L':
//            cmd_fingerprint_ledtest();
//            break;
#endif // FINGERPRINT
        case 's': i2c_scanner(&Wire1); break;
        default:
            Serial.println(F("c = create model"));
            Serial.println(F("d = OLED demo"));
            Serial.println(F("e = enroll"));
            Serial.println(F("C = config"));
            Serial.println(F("g = get"));
            Serial.println(F("i = init"));
            Serial.println(F("o = poll"));
            Serial.println(F("p = sensor OFF"));
            Serial.println(F("P = sensor ON"));
            Serial.println(F("r = redisplay"));
            Serial.println(F("s = i2c scanner"));
            Serial.println(F("t = template count"));
            Serial.println(F("f = fastsearch"));
            break;
//        case 'w': 
//            cmd_keyboard_write();
//            break;
//
//        case 'r':
//            cmd_record_password();
//            break;
//
//        default:
//            Serial.println("iflwr");
//            break;
    }
}

void setup(void)
{
    memset(&config, 0, sizeof(config));
    Serial.begin(115200);

#ifdef FINGERPRINT
    Serial1.begin(57600);
#endif // FINGERPRINT
    pinMode(PIN_FINGERPRINT_POWER, OUTPUT);
    power_off();

    Wire1.setSDA(PIN_OLED_SDA);
    Wire1.setSCL(PIN_OLED_SCL);
    Wire1.begin();
    Wire.begin();

    pinMode(PIN_TOUCH, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_TOUCH), touched, RISING);

//    at.setTimeout(click_timeout, 500);
}

// void click_timeout()
// {
//     at.setTimeout(click_timeout, 500);
//     if (2 == config.numClicks) // double click
//     {
//         power_on();
//         delay(300);
//         cmd_fingerprint_init();
//         cmd_fingerprint_get_id(); 
//         at.setTimeout(power_off, 1000);
//         config.numClicks = 0;
//         return;
//     }
//     else if (1 == config.numClicks) // single click
//     {
//         Serial.print('S');
//         config.numClicks = 0;
//         return;
//     }
// }

void handle_touched()
{
//    config.numClicks++;
    power_on();
    delay(300);
    cmd_fingerprint_init();
    int8_t results = cmd_fingerprint_get_id();
    Serial.print(F("cmd_fingerprint_get_id "));
    Serial.println((int)results);
//    at.setTimeout([](){ digitalWrite(PIN_FINGERPRINT_POWER, 0); }, 500);
    if (results > -1)
    {
        Serial.println(F("Run demo"));
        cmd_oled_demo();
        power_off();
    }
    else
    {
        power_off();
    }
}

void loop(void)
{
    at.handle();
    if (true == config.finger_touch)
    {
        handle_touched();
//        cmd_fingerprint_touched();
//#ifdef FINGERPRINT
//        cmd_fingerprint_get_id();
//#endif // FINGERPRINT
//        digitalWrite(PIN_FINGERPRINT_POWER, 0);
//        delay(100);
//        Serial.println(finger.fingerFastSearch());
//        digitalWrite(PIN_FINGERPRINT_POWER, 1);
        config.finger_touch = false;
    }
    if (Serial.available())
    {
        handleCommand(Serial.read());
//        handleCommand(Serial.read());
    }
}

