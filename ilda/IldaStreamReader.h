#ifndef ILDASTREAMREADER_H

#define ILDASTREAMREADER_H

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <cstdlib>
#include "endian.h"
#include "IldaModels.h"

class IldaStreamReader
{

    protected: 

        IldaFrame _currentFrame;        
        uint8_t * _currentRecord;

        int _currentElementPosition;
        int _currentElementType;
        bool _isCurrentElementRead;
        int _currentRecordNumber;

        int _lastReadElementType;

        const void * _rawData;
        int _rawDataPosition;
        int _rawDataLength;

    public:
        static const int ERROR_SUCCESS = 0;
        static const int ERROR_NOT_ENOUGHT_DATA = 1;
        static const int ERROR_NO_MORE_FRAMES = 1;

        static const int ELEMENTTYPE_NONE   = 0;
        static const int ELEMENTTYPE_FRAME  = 1;
        static const int ELEMENTTYPE_RECORD = 2;

        static const int RECORD_BUFFER_SIZE = sizeof(IldaRecordF4);

        IldaStreamReader();

        int GetFrame(IldaFrame * outIldaFrame);

        int GetLastReadElementType(int * elementType);
        
        int GetRecord(void * outIldaRecord);

        int Read();

        bool IsDataEnd();

        void Reset();

        int SetRawData(const void * inRawData, int lenght);

};

#endif