
#ifndef ILDAATMEGA32U4BUFOUTPUT_H

#define ILDAATMEGA32U4BUFOUTPUT_H

#include "IldaProcessor.h"

class IldaAtmega32u4BufOutput: public IldaOutputBase
{
    public:
        void Init();

        void SetCoords(int16_t x, int16_t y, int16_t z) override;
        void SetColor(uint8_t r, uint8_t g, uint8_t b) override;
        void SetBlank(bool value) override;
        void Out() override;

        void SetAndOut(int16_t x, int16_t y, int16_t z, uint8_t r, uint8_t g, uint8_t b, bool blank) override;

        void NoDataLed(bool newState) override;

    protected:

        int16_t _x;
        int16_t _y;
        int16_t _z;

        int16_t _r;
        int16_t _g;
        int16_t _b;

        bool _blanking;

        bool _noDataLedState = false;

        void portBDout(byte data);
        void portFout(byte data);
};


#endif