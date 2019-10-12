
#ifndef ILDAESP32_H

#define ILDAESP32_H

#include "IldaProcessor.h"
// #include <HardwareSerial.h>
#include <USBAPI.h>

class IldaSerialDataReader: public IldaDataReaderBase
{
    public:
        // IldaSerialDataReader(HardwareSerial * stream);
        IldaSerialDataReader(Serial_ * stream);

        void Init() override;

        int Read(uint8_t * buf, int bufSize) override;
        void ResetToBegin() override;
    
    protected:
    
        // HardwareSerial * _stream;
        Serial_ * _stream;

};


#endif