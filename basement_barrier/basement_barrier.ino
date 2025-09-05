#include <Wire.h>
#include <Arduino.h>
#include <stdlib.h>
#include <Wire.h>
#include <AsyncTimer.h>
AsyncTimer at;
//void help();
//extern Command commands_main[];
#define FSH(s) reinterpret_cast<const __FlashStringHelper *>(s)

#define PIN_RELAY_EXTEND 13
#define PIN_COMMAND_EXTEND 14

#define PIN_RELAY_RETRACT  5
#define PIN_COMMAND_RETRACT 2

#define PIN_SENSE_RETRACTED 12
#define PIN_SENSE_EXTENDED 4

typedef struct
{
    char key;
    const __FlashStringHelper * help;
    void (*fun)();
}Command;


struct {
    int32_t param;
    uint8_t system_state;
    uint16_t timer_id;
}config;


void setup(void);
void help(void);
Command * commandset;

const char help_help[] PROGMEM="help";
const char help_extend[] PROGMEM="extend";
const char help_retract[] PROGMEM="retract";
const char help_stop[] PROGMEM="stop";
const char empty[] PROGMEM="";
const char dashes[] PROGMEM="-------------------------------------------------------\r\n";

void help()
{
    Serial.printf("%s\r\n", FSH(dashes));
    for (Command * iter = commandset; iter->key != '&'; iter++)
    {
        Serial.printf("%c: %s\r\n", iter->key, iter->help);
    }
}
   
void cmd_extend_on()
{
    pinMode(PIN_RELAY_EXTEND, OUTPUT);
    digitalWrite(PIN_RELAY_EXTEND, LOW);
}

void cmd_extend_off()
{
    pinMode(PIN_RELAY_EXTEND, INPUT);
}

void cmd_retract_on()
{
    pinMode(PIN_RELAY_RETRACT, OUTPUT);
    digitalWrite(PIN_RELAY_RETRACT, LOW);
}

void cmd_retract_off()
{
    pinMode(PIN_RELAY_RETRACT, INPUT);
}

void set_bit(byte bitnum, bool value)
{
//    Serial.printf("set_bit %u -> %u\r\n", bitnum, value);
//    Serial.println(config.system_state, 2);
    config.system_state &= ~(byte)(1 << bitnum);
    if (value)
    {
        config.system_state |= (byte)(1 << bitnum);
    }
//    Serial.println(config.system_state, 2);
}

bool get_bit(byte bitnum)
{
    return config.system_state & (byte)1 << bitnum;
}

// Extend Extended Retract Retracted Timeout
const byte EXTEND = 3;
const byte EXTENDED = 2;
const byte RETRACT = 1;
const byte RETRACTED = 0;

void (*RelayState[32][2])() = {        // EeRr
    {cmd_extend_off, cmd_retract_off}, // 0000
    {cmd_extend_off, cmd_retract_off}, // 0001
    {cmd_extend_off, cmd_retract_on},  // 0010
    {cmd_extend_off, cmd_retract_off}, // 0011
    {cmd_extend_off, cmd_retract_off}, // 0100
    {cmd_extend_off, cmd_retract_off}, // 0101
    {cmd_extend_off, cmd_retract_on},  // 0110
    {cmd_extend_off, cmd_retract_off}, // 0111
    {cmd_extend_on, cmd_retract_off},  // 1000
    {cmd_extend_on, cmd_retract_off},  // 1001
    {cmd_extend_off, cmd_retract_off}, // 1010
    {cmd_extend_off, cmd_retract_off}, // 1011
    {cmd_extend_off, cmd_retract_off}, // 1100
    {cmd_extend_off, cmd_retract_off}, // 1101
    {cmd_extend_off, cmd_retract_off}, // 1110
    {cmd_extend_off, cmd_retract_off}, // 1111
};

Command commands_main[] = {
{'e',FSH(help_extend), [](){ config.system_state |= EXTEND;}},
{'r',FSH(help_retract), [](){ config.system_state |= RETRACT; }},
{' ',FSH(help_stop), [](){ config.system_state = 0; }},
{'h',FSH(help_help), [](){ help();}},
{'&', FSH(empty), [](){} }
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

    if (command >= '0' & command <= '9')
    {
        config.param *= 10;
        config.param += command - '0';
    }
}

void setup(void)
{
    memset(&config, 0, sizeof(config));
//    Serial.begin(115200);
    commandset = commands_main;

    pinMode(PIN_COMMAND_EXTEND, INPUT_PULLUP);
    pinMode(PIN_COMMAND_RETRACT, INPUT_PULLUP);
    pinMode(PIN_SENSE_RETRACTED, INPUT_PULLUP);
    pinMode(PIN_SENSE_EXTENDED, INPUT_PULLUP);
}

void loop(void)
{
    at.handle();
    if (Serial.available())
    {
        handleCommand(Serial.read());
    }

    set_bit(RETRACTED, !digitalRead(PIN_SENSE_RETRACTED));
    set_bit(EXTENDED, !digitalRead(PIN_SENSE_EXTENDED));
    bool extend = !digitalRead(PIN_COMMAND_EXTEND);
    bool retract = !digitalRead(PIN_COMMAND_RETRACT);
//    Serial.printf("extend %u retract %u remaining %u\r\n", extend, retract, 
//        config.timer_id ? at.getRemaining(config.timer_id) : 0);

    if (extend && retract)
    {
        set_bit(EXTEND, 0);
        set_bit(RETRACT, 0);
        RelayState[config.system_state][0]();
        RelayState[config.system_state][1]();
        if (config.timer_id)
        {
            at.cancel(config.timer_id);
        }
        delay(500);
    }
    else if (extend && !get_bit(EXTEND) && !get_bit(RETRACT) && !get_bit(EXTENDED))
    {
        set_bit(EXTEND, 1);
        at.cancel(config.timer_id);
        config.timer_id = at.setTimeout([](){ config.system_state = 0; }, 30000);
    }
    else if (retract && !get_bit(RETRACT) && !get_bit(EXTEND) && !get_bit(RETRACTED))
    {
        set_bit(RETRACT, 1);
        at.cancel(config.timer_id);
        config.timer_id = at.setTimeout([](){ config.system_state = 0; }, 30000);
    }
    if (get_bit(RETRACTED))
    {
        set_bit(RETRACT, 0);
    }
    if (get_bit(EXTENDED))
    {
        set_bit(EXTEND, 0);
    }
//    Serial.println(config.system_state, 2);
//    delay(1000);
    RelayState[config.system_state][0]();
    RelayState[config.system_state][1]();
}


