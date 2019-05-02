
#ifndef ILDAESP32OUTPUT_H

#define ILDAESP32OUTPUT_H

#include "IldaProcessor.h"

class IldaEsp32Output: public IldaOutputBase
{
    public:
        void Init();

        void SetCoords(int16_t x, int16_t y, int16_t z) override;
        void SetColor(uint8_t r, uint8_t g, uint8_t b) override;
        void SetBlank(bool value) override;
        void Out() override;

        void SetAndOut(int16_t x, int16_t y, int16_t z, uint8_t r, uint8_t g, uint8_t b, bool blank) override;

    protected:
        const int _pinBlanking = 33;
        const int _pinBlankingEnable = HIGH;
        const int _pinBlankingDisable = LOW;

        const int _pinDacX = 25;
        const int _pinDacY = 26;

        int16_t _x;
        int16_t _y;
        int16_t _z;

        int16_t _r;
        int16_t _g;
        int16_t _b;

        bool _blanking;
};


#endif