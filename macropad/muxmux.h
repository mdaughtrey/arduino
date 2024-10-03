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
// #include <vector>

class OneMux 
{
    public:
        uint8_t _address;
        TwoWire * _i2c;
        OneMux(uint8_t address, TwoWire & wirePort = Wire);
//        bool setPorts(uint8_t ports);
//        int8_t getPort();
        bool write(uint8_t data);
        uint8_t read();
};

class MuxMux : public TwoWire
{
    public:
//        OneMux * _mux0;
//        OneMux * _mux1;
        uint8_t _currentPort;

        MuxMux(uint8_t bus_num);
        bool setPort(uint8_t port);
  //      uint8_t getPorts();
        size_t write(uint8_t data);
        int read();
        void reset(bool state);
};
