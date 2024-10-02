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
    _currentPort = -1;
}

bool OneMux::setPort(int8_t port)
{ 
    _i2c->beginTransmission(address);
    _i2c->write(1 << port);
    if (_i2c->endTransmission())
    {
        return false;
    }
    _currentPort = port;
    return true; 
}

int8_t OneMux::getPort()
{ 
    return _currentPort;
}

bool OneMux::set(uint8_t data)
{ 

    return true; 
}

uint8_t OneMux::get()
{ 
    return 0; 
}

bool OneMux::enable(int8_t portNumber)
{ 
    return true;
}

bool OneMux::disable(int8_t portNumber)
{ 
    return true;
}


MuxMux::MuxMux(TwoWire & wirePort)
{
    _mux0 = new OneMux(ADDR_MUX0, Wire);
    _mux1 = new OneMux(ADDR_MUX1, Wire);
}

bool MuxMux::setPort(int8_t port)
{ 
    if (port > 15)
    {
        return false;
    }

    if (port > 8)
    {
        _mux1->setPort(port - 8);
        return true; 
    }
    _mux0->setPort(port);
    _currentPort = port;
    return true; 
}

int8_t MuxMux::getPort()
{ 
    return _currentPort; 
}

bool MuxMux::set(uint8_t data)
{ 
    return true; 
}

uint8_t MuxMux::get()
{ 
    return 0; 
}

bool MuxMux::enable(int8_t portNumber)
{ 
    return true;
}

bool MuxMux::disable(int8_t portNumber)
{ 
    return true;
}

void MuxMux::reset(bool state)
{
    // digitalWrite
}


