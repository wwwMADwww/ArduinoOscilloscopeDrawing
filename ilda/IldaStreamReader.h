#ifndef ILDASTREAMREADER_H

#define ILDASTREAMREADER_H

#include <string.h>
#include <stdint.h>
#include <cstdlib>
#include "endian.h"
#include "IldaModels.h"

class IldaStreamReader
{

    public:
        static const int ERROR_SUCCESS = 0;
        static const int ERROR_NOT_ENOUGHT_DATA = 1;
        static const int ERROR_INVALID_RECORD_FORMAT = 2;

        static const int ELEMENTTYPE_NONE   = 0;
        static const int ELEMENTTYPE_FRAME  = 1;
        static const int ELEMENTTYPE_RECORD = 2;

        static const int RECORD_BUFFER_SIZE = sizeof(IldaRecordF4); // IldaRecordF4 largest record, 10 bytes


        IldaStreamReader();

        int GetFrame(IldaFrame * outIldaFrame);

        int GetLastReadElementType(int * outElementType);
        
        int GetRecord(uint8_t * outIldaRecord);

        int Read();

        bool IsDataEnd();

        void Reset();

        int SetRawData(const uint8_t * inRawData, int rawDataSize);


    protected: 
        IldaFrame _currentFrame;
        uint8_t _currentRecord[RECORD_BUFFER_SIZE];

        int _currentElementPosition;
        int _currentElementType;
        bool _isCurrentElementRead;
        int _currentRecordNumber;

        int _lastReadElementType;

        const uint8_t * _rawData;
        int _rawDataPosition;
        int _rawDataSize;

};

#endif