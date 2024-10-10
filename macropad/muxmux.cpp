/*
 * =====================================================================================
 *
 *       Filename:  muxmux.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/01/2024 10:16:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "muxmux.h"

const int ADDR_MUX0 = 0x70;
const int ADDR_MUX1 = 0x71;

OneMux::OneMux(uint8_t address, TwoWire & wirePort)
{
    _i2c = &wirePort;
    _address = address;
}

//bool OneMux::setPorts(int8_t port)
//{ 
//    _i2c->beginTransmission(address);
//    _i2c->write(1 << port);
//    if (_i2c->endTransmission())
//    {
//        return false;
//    }
//    _currentPort = port;
//    return true; 
//}
//
//int8_t OneMux::getPort()
//{ 
//    return _currentPort;
//}

bool OneMux::write(uint8_t data)
{ 
    Serial.printf(F("OneMux::write %02x\r\n"), data);
    return true; 
}

uint8_t OneMux::read()
{ 
    Serial.printf(F("OneMux::read\r\n"));
    return 0; 
}

// bool OneMux::enable(int8_t portNumber)
// { 
//     return true;
// }
// 
// bool OneMux::disable(int8_t portNumber)
// { 
//     return true;
// }


MuxMux::MuxMux(uint8_t bus_num)
    : TwoWire(bus_num)
{
//    _mux0 = new OneMux(ADDR_MUX0, Wire);
//    _mux1 = new OneMux(ADDR_MUX1, Wire);
}

bool MuxMux::setPort(uint8_t port)
{ 
    if (port > 15)
    {
        return false;
    }
    if (port > 7)
    {
        _muxAddress = ADDR_MUX1;
        _currentPort = port - 8;
    }
    else 
    {
        _muxAddress = ADDR_MUX0;
        _currentPort = port;
    }
    beginTransmission(_muxAddress);
    TwoWire::write(1 << port);
    endTransmission();
    return true;

}

size_t MuxMux::write(uint8_t data)
{ 
    Serial.printf(F("MuxMux::write %02x\r\n"), data);
    TwoWire::write(data);
    return true; 
}

size_t MuxMux::write(const uint8_t * data, size_t size)
{ 
    Serial.printf(F("MuxMux::write %02x bytes\r\n"), size);
    TwoWire::write(data,size);
    return true; 
}

int MuxMux::read()
{ 
    Serial.printf(F("MuxMux::read\r\n"));
    beginTransmission(_muxAddress);
    requestFrom(_muxAddress, uint8_t(1));
    uint8_t response = TwoWire::read();
    endTransmission();
    Serial.printf(F("MuxMux::read Address %02x returns %02x\r\n"), _muxAddress, response);
    return response;
}

// bool MuxMux::enable(int8_t portNumber)
// { 
//     return true;
// }
// 
// bool MuxMux::disable(int8_t portNumber)
// { 
//     return true;
// }

void MuxMux::reset(bool state)
{
    // digitalWrite
}


bool MuxMux::isConnected()
{
    beginTransmission(_muxAddress);
    if (0 != endTransmission())
    {
        Serial.printf(F("MuxMux::isConnected: no device ACK from mux %02x"), _muxAddress);
        return false;
    }
    TwoWire::write(0xa4);
    requestFrom(_muxAddress, uint8_t(1));
    uint8_t response = TwoWire::read();
    TwoWire::write(0x00);
    if (0xa4 != response)
    {
        Serial.printf(F("MuxMux::isConnected: register readback fails from mux %02x"), _muxAddress);
        return false;
    }
    endTransmission();
    return true;
}
