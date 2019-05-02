
#ifndef ILDASDCARDREADER_H

#define ILDASDCARDREADER_H

#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include "IldaProcessor.h"

class IldaSDCardDataReader: public IldaDataReaderBase
{
    public:
        void Init() override;

        int Read(uint8_t * buf, int bufSize) override;
        void ResetToBegin() override;
    
    protected:
    
        int _sdCardWaitTime = 3000;

        int _fileReadBufSize = 1024 * 10;

        int _filenameLen = 255;
        char * _filename;

        File _file;
        // int _fileSize = 0;
};


#endif