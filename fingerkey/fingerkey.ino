#include <Arduino.h>
#include <SoftwareSerial.h>
#include <stdint.h>
#include <Wire.h>
#include <EEPROM.h>
//#include <Preferences.h>
#include <Keyboard.h>
#ifdef FINGERPRINT
#include <Adafruit_Fingerprint.h>
#endif // FINGERPRINT

// Fingerprint Sensor
// Red VTouch
// Black TouchOut
// Yellow 3v3
// Green  tx
// Blue Rx
// White Gnd


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
//    bool accumParameter;
}config;

#ifdef FINGERPRINT
SoftwareSerial FSerial = SoftwareSerial(1,0);
// Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial1);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&FSerial);
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
void cmd_fingerprint_enroll();
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

void cmd_fingerprint_init()
{
  // set the data rate for the sensor serial port
//    Serial.println(F("Sensor Check 1"));
//  Serial1.begin(57600);
//    Serial.println(F("Sensor Check 2"));
//  finger.begin(57600);
//    Serial.println(F("Sensor Check 3"));
    delay(1000);

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
//    Serial.printf(F("Deleting %d\r\n"), id);
    byte p = finger.deleteModel(id);
    fingerprint_error(p);
}

void cmd_fingerprint_clear()
{
    finger.emptyDatabase();
}

void cmd_fingerprint_enroll(int32_t id)
{
    int p = -1;
//    Serial.printf(F("Waiting for valid finger to enroll as #%u\r\n"), id);
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
//    Serial.printf(F("ID %u\r\n"), id);
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
//    Serial.printf(F("Creating model for id %d\r\n"), id);

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

const char message[] PROGMEM = "Hello";

void cmd_keyboard_write()
{
    byte index = 0;
    Serial.println("write");
    Keyboard.begin();
//    Keyboard.releaseAll();
    while (1)
    {
        char cc = EEPROM.read(index++);
        if (cc == '\0')
            break;
        Serial.print(cc);
        Keyboard.write(cc);
        delay(100);
    }
    Keyboard.end();
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

void handleCommand(byte command)
{
    switch (command)
    {
        case 'w': 
            cmd_keyboard_write();
            break;

        case 'r':
            cmd_record_password();
            break;
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
//    command_set = commands_main;
    Serial.begin(115200);
#ifdef FINGERPRINT
    FSerial.begin(57600);
#endif // FINGERPRINT
//    Serial1.begin(1200);
}

void loop(void)
{
//    Serial1.println(F("Hello"));
//    Serial.println(F("Hello"));
//    delay(1000);
    if (Serial.available())
    {
        handleCommand(Serial.read());
#ifdef FINGERPRINT
        cmd_fingerprint_init();
#endif // FINGERPRINT
//        handleCommand(Serial.read());
    }
}

