
#include "IldaAtmega32u4BufOutput.h"

void IldaAtmega32u4BufOutput::Init()
{
    // arduino pro micro

    pinMode(PD2, OUTPUT);
    pinMode(PD3, OUTPUT);
    pinMode(PD4, OUTPUT);
    pinMode(PD5, OUTPUT);
    pinMode(PD6, OUTPUT);
    pinMode(PD7, OUTPUT);
    pinMode(  4, OUTPUT);
    pinMode(  6, OUTPUT);
    pinMode(  8, OUTPUT);
    pinMode(  9, OUTPUT);
    pinMode( 10, OUTPUT);
    pinMode( 14, OUTPUT);
    pinMode( 15, OUTPUT);
    pinMode( 16, OUTPUT);
    
    pinMode(A0, OUTPUT);
    pinMode(A1, OUTPUT);
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);

    pinMode( 5, OUTPUT);

    SetAndOut(0, 0, 0, 0, 0, 0, true);
}


void IldaAtmega32u4BufOutput::SetCoords(int16_t x, int16_t y, int16_t z)
{
    _x = x;
    _y = y;
    _z = z;
}

void IldaAtmega32u4BufOutput::SetColor(uint8_t r, uint8_t g, uint8_t b)
{
    _r = r;
    _g = g;
    _b = b;
}

void IldaAtmega32u4BufOutput::SetBlank(bool value)
{
    _blanking = value;
}


void IldaAtmega32u4BufOutput::SetAndOut(
    int16_t x, int16_t y, int16_t z, 
    uint8_t r, uint8_t g, uint8_t b, 
    bool blank)
{
    _x = x;
    _y = y;
    _z = z;

    _r = r;
    _g = g;
    _b = b;

    _blanking = blank;
    
    Out();
}

void IldaAtmega32u4BufOutput::Out()
{

    byte x = (_x + 32768) / 256;
    byte y = (_y + 32768) / 256;
    byte z = (_z + 32768) / 256;

    // see the diagrams

    // ports B and D are used for data output
    // port F is selecting register for storing data from ports B and D (values from 1 << 0 to 1 << 2)
    // port F is also used to allow stored data output from registers to DACs (value 1 << 3)

    portFout(1 << 0);
    portBDout(x);

    portFout(1 << 1);
    portBDout(y);

    portFout(1 << 2);
    // portBDout(_z);
    portBDout(_blanking ? 0xFF : 0x00);

    portFout(1 << 3);
}

void IldaAtmega32u4BufOutput::NoDataLed(bool newState)
{
    if (newState == _noDataLedState)
        return;
    
    _noDataLedState = newState;
    digitalWrite(5, _noDataLedState ? HIGH : LOW);
}

void IldaAtmega32u4BufOutput::portBDout(byte data)
{

    // atmega32u4

    int portb = PORTB & 0b10000001;
    portb |= (data << 1) & 0b01111110;

    int portd = PORTD & 0b01101111;
    portd |= ((data & 0b10000000) | (data >> 2)) & 0b10010000;

    {
    PORTB = portb;
    PORTD = portd;
    }

}

void IldaAtmega32u4BufOutput::portFout(byte data)
{

    // atmega32u4

    int portf = PORTF & 0b00001111;
    portf |= (data << 4) & 0b11110000;

    PORTF = portf;
}