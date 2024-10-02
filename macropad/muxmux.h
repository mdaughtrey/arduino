/*
 * =====================================================================================
 *
 *       Filename:  muxmux.h
 *
 *    Description: Multiplexes 2 PCA9548 I2C Multiplexers
 *
 *        Version:  1.0
 *        Created:  10/01/2024 09:58:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdint.h>
#include <Arduino.h>
#include <Wire.h>

class OneMux
{
    public:
        int8_t _currentPort;
        uint8_t _address;
        TwoWire * _i2c;
        OneMux(uint8_t address, TwoWire & wirePort = Wire);
        bool setPort(int8_t port);
        int8_t getPort();
        bool set(uint8_t data);
        uint8_t get();
        bool enable(int8_t portNumber = -1);
        bool disable(int8_t portNumber = -1);
};

class MuxMux
{
    public:
        OneMux * _mux0;
        OneMux * _mux1;
        int8_t _currentPort;

        MuxMux(TwoWire & wirePort = Wire);
        bool setPort(int8_t port);
        int8_t getPort();
        bool set(uint8_t data);
        uint8_t get();
        bool enable(int8_t portNumber = -1);
        bool disable(int8_t portNumber = -1);
        void reset(bool state);
};
