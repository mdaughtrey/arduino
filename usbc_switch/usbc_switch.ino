#include <Wire.h>
#include <Arduino.h>
#include <stdlib.h>
#include <Wire.h>
#include <AsyncTimer.h>
#include <BleSerial.h>
// #include <BLEDevice.h>
// #include <BLEUtils.h>
// #include <BLEServer.h>

//AsyncTimer at;
//void help();
//extern Command commands_main[];
#define FSH(s) reinterpret_cast<const __FlashStringHelper *>(s)

#define SERVICE_UUID "a33ac316-2721-43be-8e35-86f3f5df0579"
#define CHARACTERISTIC_UUID "c7bdf54d-e98f-4934-b511-c627b262c801"


typedef struct
{
    char key;
    const __FlashStringHelper * help;
    void (*fun)();
}Command;

struct {
    int32_t param;
    int8_t input_channel;
}config;

void setup(void);
void help(void);
Command * commandset;

const uint8_t PIN_AA = 16;  // Board A Input A
const uint8_t PIN_AB = 4;  // Board A Input B
const uint8_t PIN_BA = 12;  // Board B Input A
const uint8_t PIN_BB = 14;  // Board B Input B

const uint8_t PIN_A = 17;  // Board A button
const uint8_t PIN_B = 27;  // Board B button


const char help_paramneg1[] PROGMEM="param *= -1";
const char help_help[] PROGMEM="help";
const char help_togglea[] PROGMEM="Toggle A";
const char help_toggleb[] PROGMEM="Toggle B";
const char help_param0[] = "Clear parameter";
const char help_select[] = "Select";
const char empty[] PROGMEM="";
const char dashes[] PROGMEM="-------------------------------------------------------\r\n";

BleSerial ble;

bool aa()
{
    return digitalRead(PIN_AA);
}

bool ab()
{
    return digitalRead(PIN_AB);
}

bool ba()
{
    return digitalRead(PIN_BA);
}

bool bb()
{
    return digitalRead(PIN_BB);
}


void help()
{
    Serial.printf("%s\r\n", FSH(dashes));
    for (Command * iter = commandset; iter->key != '&'; iter++)
    {
        Serial.printf("%c: %s\r\n", iter->key, iter->help);
    }
    Serial.printf("AA %d AB %d BA %d BB %d\r\n",
        aa(), ab(), ba(), bb());
}
   
void togglea()
{
    pinMode(PIN_A, OUTPUT);
    digitalWrite(PIN_A, 0);
    delay(100);
    digitalWrite(PIN_A, 1);
    pinMode(PIN_A, INPUT);
}

void toggleb()
{
    pinMode(PIN_B, OUTPUT);
    digitalWrite(PIN_B, 0);
    delay(100);
    digitalWrite(PIN_B, 1);
    pinMode(PIN_B, INPUT);
}

void channel_select(uint8_t channel)
{
    switch (channel)
    {
        case 0:
            while (!(!aa() && ab()))
            {
                togglea();
            }
            break;
        case 1:
            while (!(aa() && !ab()))
            {
                togglea();
            }
            while (!(ba() && !bb()))
            {
                toggleb();
            }
            break;
        case 2:
            while (!(aa() && !ab()))
            {
                togglea();
            }
            while (!(!ba() && bb()))
            {
                toggleb();
            }
            break;
        default: 
            break;
    }
}

Command commands_main[] = {
{'-',FSH(help_paramneg1), [](){ config.param *= -1;}},
{'a',FSH(help_togglea), [](){ togglea();}},
{'b',FSH(help_toggleb), [](){ toggleb();}},
{'c',FSH(help_param0), [](){ config.param = 0;}},
{'h',FSH(help_help), [](){ help();}},
{'s',FSH(help_select), [](){ channel_select(config.param);
        config.param = 0;}},
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
    Serial.begin(115200);
    commandset = commands_main;
    pinMode(PIN_AA, INPUT_PULLUP);
    pinMode(PIN_AB, INPUT_PULLUP);
    pinMode(PIN_BA, INPUT_PULLUP);
    pinMode(PIN_BB, INPUT_PULLUP);

    ble.begin("USBC Switcher");

//    BLEDevice::init("USBC Switcher");
//    BLEServer *pServer = BLEDevice::createServer();
//    BLEService *pService = pServer->createService(SERVICE_UUID);
//    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
//        CHARACTERISTIC_UUID,
//        BLECharacteristic::PROPERTY_READ |
//        BLECharacteristic::PROPERTY_WRITE);
//    pCharacteristic->setValue("channel");
//    pService->start();
//
//    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
//    pAdvertising->addServiceUUID(SERVICE_UUID);
//    pAdvertising->setScanResponse(true);
//    pAdvertising->setMinPreferred(0x06);
//    pAdvertising->setMinPreferred(0x12);
//    BLEDevice::startAdvertising();

}

void loop(void)
{
    if (Serial.available())
    {
        handleCommand(Serial.read());
    }
    if (ble.available())
    {
        handleCommand(ble.read());
    }
}

