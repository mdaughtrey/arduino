#include <Arduino.h>
//#include <YetAnotherPcInt.h>
//#include <SoftwareSerial.h>
#include <stdint.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <EEPROM.h>
//#include <Preferences.h>
//#include <Keyboard.h>
#define FINGERPRINT
#ifdef FINGERPRINT
#include <Adafruit_Fingerprint.h>
#endif // FINGERPRINT


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

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);

const uint8_t PIN_TOUCH = 2;
const uint8_t PIN_FINGERPRINT_POWER = 15;

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
}config;



#ifdef FINGERPRINT
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);
#endif // FINGERPRINT

// Command * command_set;
// extern Command commands_main[];
// extern Command commands_fingerprint[];

// void handleCommand(byte command);
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

//const char dashes[] PROGMEM="-------------------------------------------------------\r\n";

// void cmd_help()
// {
//     Serial.println(F("-------------------------------------------------------"));
//     for (Command * iter = command_set; iter->key != '&'; iter++)
//     {
//         Serial.printf(F("%c: %s\r\n"), iter->key, iter->help);
//     }
// }

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


// Command commands_main[] = {
// {'?',F("Show Configuration"), [](){ cmd_show_config();}},
// {'c',F("Clear Numeric Parameter"), [](){ cmd_clear_numeric_parameter();}},
// //{'C',F("Clear Keystore"), [](){ cmd_clear_keystore();}},
// #ifdef FINGER
// {'f',F("Fingerprint Menu"), [](){ cmd_fingerprint_menu(); }},
// #endif // FINGER
// //{'f',F("Font Select"), [](){ cmd_face_select();}},
// {'h',F("Help"), [](){ cmd_help();}},
// //{'l',F("Set Label"), [](){ cmd_set_label();}},
// //{'m',F("Set Macro"), [](){ cmd_set_macro();}},
// //{'p',F("Input Parameter"), [](){ cmd_input_parameter(); }},
// //{'s',F("Serial2 test string"), [](){ cmd_serial2_test_string(); }},
// //{'s',F("SPIFFS Test"), [](){ cmd_spiffs_test(); }},
// //{'u',F("Upload key definitions"), [](){ cmd_upload_keydefs(); }},
// //{'y',F("Upload YAML File"), [](){ cmd_upload_yaml(); }},
// {' ',F("Reset"), [](){ cmd_reset(); }},
// {'&',F("Reset"), [](){ cmd_reset(); }}
// };

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

// void cmd_fingerprint_menu(void)
// {
//     command_set = commands_fingerprint;
// }

//    attachInterrupt(digitalPinToInterrupt(FilmSensorPin8mm), isr1, RISING);
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

void cmd_fingerprint_get_id()
{
    byte p = finger.getImage();
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
//    Serial.printf(F("Found ID %d with confidence %d\r\n"), finger.fingerID, finger.confidence);
}
#endif // FINGERPRINT

// Command commands_fingerprint[] = {
// {'c',F("Clear Numeric Parameter"), [](){ cmd_clear_numeric_parameter(); }},
// {'C',F("Clear Fingerprint DB"), [](){ cmd_fingerprint_clear(); }},
// {'e',F("Enroll new fingerprint to id (parameter)"), [](){ cmd_fingerprint_enroll(config.numericParam); }},
// {'h',F("Help"), [](){ cmd_help(); }},
// {'i',F("Init"), []() { cmd_fingerprint_init(); }},
// {'l',F("LED Test"), []() { cmd_fingerprint_ledtest(); }},
// {'r',F("Read Fingerprint"), []() { cmd_fingerprint_get_id(); }},
// {'x',F("Return to main menu"), []() { command_set = commands_main; }},
// {'&',F("Reset"), [](){ cmd_reset(); }}
// };

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
    digitalWrite(PIN_FINGERPRINT_POWER, 1);
    delay(200);
    cmd_fingerprint_get_id();
    delay(500);
    digitalWrite(PIN_FINGERPRINT_POWER, 0);
}

void cmd_oled_demo()
{
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
    {
        Serial.println(F("SSD1306 allocation failed"));
        return;
    }
    Serial.println(F("SSD1305 Init Ok"));
    display.display();
}

void handleCommand(byte command)
{
    switch (command)
    {
//        case 'i': 
//            i2c_scanner();
//            break;
        case 'o': cmd_poll(); break;
#ifdef FINGERPRINT
        case 'c': fingerprint_error(finger.createModel()); break;
        case 'd': cmd_oled_demo(); break;
        case 'e': cmd_fingerprint_enroll(0); break;
        case 'C': cmd_show_config(); break;
        case 'g': cmd_fingerprint_get_id(); break;
        case 'i': cmd_fingerprint_init(); break;
        case 'p': digitalWrite(PIN_FINGERPRINT_POWER, 0); break;
        case 'P': digitalWrite(PIN_FINGERPRINT_POWER, 1); break;
        case 's': i2c_scanner(&Wire1); break;
        case 't': cmd_fingerprint_touched(); break;
        case 'T': Serial.println(finger.getTemplateCount()); break;
        case 'f': Serial.println(finger.fingerFastSearch()); break;
        case 'x':
            Serial.print(F("SDA: "));
            Serial.println(PIN_WIRE1_SDA);
            Serial.print(F("SCL: "));
            Serial.println(PIN_WIRE1_SCL);
            break;
        
//        case 'L':
//            cmd_fingerprint_ledtest();
//            break;
#endif // FINGERPRINT
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


//    if (config.accumParameter == true)
//    {
//        accumulate_parameter(command);
//        return;
//    }
//    for (Command * iter = command_set; iter->key != '&'; iter++)
//    {
//        if (iter->key == command)
//        {
//            iter->fun();
//            return;
//        }
//    }
//    if (command >= '0' & command <= '9')
//    {
//        config.numericParam *= 10;
//        config.numericParam += command - '0';
//    }
// }


void setup(void)
{
    memset(&config, 0, sizeof(config));
    Serial.begin(115200);

//    PCICR |= (1 << PCIE0);
//    PCMSK0 |= (1 << PCINT6);

#ifdef FINGERPRINT
    Serial1.begin(57600);
#endif // FINGERPRINT
    pinMode(PIN_FINGERPRINT_POWER, OUTPUT);
    digitalWrite(PIN_FINGERPRINT_POWER, 0);
//    delay(100);
//    cmd_fingerprint_init();
//    delay(100);
//    digitalWrite(PIN_FINGERPRINT_POWER, 1);

    pinMode(PIN_TOUCH, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_TOUCH), touched, RISING);
//    attachInterrupt(digitalPinToInterrupt(FilmSensorPin8mm), isr1, RISING);
}

void loop(void)
{
//    Serial1.println(F("Hello"));
//    Serial.println(F("Hello"));
    if (true == config.finger_touch)
    {
        Serial.println("Touched");
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
#ifdef FINGERPRINT
//        cmd_fingerprint_init();
#endif // FINGERPRINT
//        handleCommand(Serial.read());
    }
}

