
#ifndef ILDAPROCESSOR_H

#define ILDAPROCESSOR_H

#include "Arduino.h"
#include "IldaStreamReader.h"

class IldaDataReaderBase
{
    public:
        virtual void Init();

        virtual int Read(uint8_t * buf, int bufSize);
        virtual void ResetToBegin();
};

class IldaOutputBase
{
    public:
        virtual void Init();

        virtual void SetCoords(int16_t x, int16_t y, int16_t z);
        virtual void SetColor(uint8_t r, uint8_t g, uint8_t b);
        virtual void SetBlank(bool value);
        virtual void Out();

        virtual void SetAndOut(int16_t x, int16_t y, int16_t z, uint8_t r, uint8_t g, uint8_t b, bool blank);

        virtual void NoDataLed(bool newState);
};

class IldaProcessor
{

    public:

        IldaProcessor(
            IldaDataReaderBase * ildaDataReader, 
            IldaOutputBase * ildaOutput,
            IldaStreamReader * ildaStreamReader);

        void Init();

        int Process();

        void Reset();


    protected: 
        
        IldaDataReaderBase * _ildaDataReader;
        IldaOutputBase * _ildaOutput;
        IldaStreamReader * _ildaStreamReader;
        
        int _nodatamillisStart;
        int _nodatamillisCurrent;

        // read and store
        uint8_t * _databuf = 0;
        int _dataBufSize;

        int _code;
        IldaFrame _frame;
        uint8_t * _recordBuf = 0;

        IldaRecordF2 * _colorPalette = 0;

        int _colorPaletteReadID;

        // output
        int16_t _x;
        int16_t _y;
        int16_t _z;

        int16_t _r;
        int16_t _g;
        int16_t _b;

        
        int16_t _testx = 0;

        bool _blanking;
        bool _lastPoint;

};

#endif