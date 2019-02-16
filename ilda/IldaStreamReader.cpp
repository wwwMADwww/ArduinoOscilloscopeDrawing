#include "IldaStreamReader.h"


IldaStreamReader::IldaStreamReader()
{
    Reset();
}

int IldaStreamReader::GetFrame(IldaFrame * outIldaFrame)
{
    if (_currentElementType == ELEMENTTYPE_FRAME && !_isCurrentElementRead)
        return ERROR_NOT_ENOUGHT_DATA;

    memcpy((void *)outIldaFrame, (void *)&_currentFrame, sizeof(IldaFrame));

    return ERROR_SUCCESS;
}

int IldaStreamReader::GetLastReadElementType(int * elementType)
{
    if (!_isCurrentElementRead)
        return ERROR_NOT_ENOUGHT_DATA;

    *elementType = _lastReadElementType;
    return ERROR_SUCCESS;
}

int IldaStreamReader::GetRecord(uint8_t * outIldaRecord)
{

    int len = 0;
    switch (_currentFrame.formatCode)
    {
        case 0: len = sizeof(IldaRecordF0); break;
        case 1: len = sizeof(IldaRecordF1); break;
        case 2: len = sizeof(IldaRecordF2); break;
        case 4: len = sizeof(IldaRecordF4); break;
        case 5: len = sizeof(IldaRecordF5); break;
    }


    memcpy(outIldaRecord, _currentRecord, len);

    return ERROR_SUCCESS;
}


int IldaStreamReader:: Read()
{
    if (IsDataEnd())
    {
        _lastReadElementType = ELEMENTTYPE_NONE;
        return ERROR_NOT_ENOUGHT_DATA;
    }

    int result = ERROR_SUCCESS;

    if (_currentElementType == ELEMENTTYPE_NONE)
    {
        _currentElementType = ELEMENTTYPE_FRAME;
    }
    

    if (_currentElementType == ELEMENTTYPE_FRAME)
    {

        int frameSize = sizeof(IldaFrame);
        int copySize = frameSize - _currentElementPosition;

        if ((copySize + _rawDataPosition) <= _rawDataSize)
        {
            _isCurrentElementRead = true;
            _lastReadElementType = _currentElementType;
        }
        else
        {
            copySize = _rawDataSize - _rawDataPosition;
            result = ERROR_NOT_ENOUGHT_DATA;
            _isCurrentElementRead = false;
        }

        memcpy((void *)((uint8_t *)&_currentFrame + _currentElementPosition), (void *)(_rawData + _rawDataPosition), copySize);

        _currentElementPosition = (_currentElementPosition + copySize) % frameSize;
        _rawDataPosition = _rawDataPosition + copySize;

        if (_isCurrentElementRead)
        {
            _currentFrame.numberOfRecords = REVERSE_U16BIT(_currentFrame.numberOfRecords);
            _currentFrame.frameNumber = REVERSE_U16BIT(_currentFrame.frameNumber);
            _currentFrame.totalFrames = REVERSE_U16BIT(_currentFrame.totalFrames);

            if (_currentFrame.formatCode != 0 &&
                _currentFrame.formatCode != 1 &&
                _currentFrame.formatCode != 2 &&
                _currentFrame.formatCode != 4 &&
                _currentFrame.formatCode != 5
            )
            {
                result = ERROR_INVALID_RECORD_FORMAT;
            }
            else
            {
                if (_currentFrame.totalFrames > 0)
                {
                    _currentElementType = ELEMENTTYPE_RECORD;
                }
            }
        }

    }
    else if (_currentElementType == ELEMENTTYPE_RECORD)
    {

        int recordSize = 0;

        switch (_currentFrame.formatCode)
        {
            case 0: recordSize = sizeof(IldaRecordF0); break;
            case 1: recordSize = sizeof(IldaRecordF1); break;
            case 2: recordSize = sizeof(IldaRecordF2); break;
            case 4: recordSize = sizeof(IldaRecordF4); break;
            case 5: recordSize = sizeof(IldaRecordF5); break;
        
            default:
                result = ERROR_INVALID_RECORD_FORMAT;
                break;
        }

        if (recordSize > 0)
        {

            int copySize = recordSize - _currentElementPosition;

            if ((copySize + _rawDataPosition) <= _rawDataSize)
            {
                _isCurrentElementRead = true;
                _lastReadElementType = _currentElementType;
                _currentRecordNumber++;
            }
            else
            {
                copySize = _rawDataSize - _rawDataPosition;
                result = ERROR_NOT_ENOUGHT_DATA;
                _isCurrentElementRead = false;
            }

                memcpy((void *)(_currentRecord + _currentElementPosition), (void *)(_rawData + _rawDataPosition), recordSize);
            
            _currentElementPosition = (_currentElementPosition + copySize) % recordSize;
            _rawDataPosition = _rawDataPosition + copySize;

            if (_isCurrentElementRead)
            {
                switch (_currentFrame.formatCode)
                {
                    case 0: 
                    {
                        IldaRecordF0 * record = (IldaRecordF0 *) _currentRecord;
                        record->x = REVERSE_16BIT(record->x);
                        record->y = REVERSE_16BIT(record->y);
                        record->z = REVERSE_16BIT(record->z);
                        break;
                    }

                    case 1: 
                    {
                        IldaRecordF1 * record = (IldaRecordF1 *) _currentRecord;
                        record->x = REVERSE_16BIT(record->x);
                        record->y = REVERSE_16BIT(record->y);
                        break;
                    }

                    case 4:
                    {
                        IldaRecordF4 * record = (IldaRecordF4 *) _currentRecord;
                        record->x = REVERSE_16BIT(record->x);
                        record->y = REVERSE_16BIT(record->y);
                        record->z = REVERSE_16BIT(record->z);
                        break;
                    }

                    case 5:
                    {
                        IldaRecordF5 * record = (IldaRecordF5 *) _currentRecord;
                        record->x = REVERSE_16BIT(record->x);                    
                        record->y = REVERSE_16BIT(record->y);
                        break;
                    }
                
                }
            }

            if (_currentRecordNumber == _currentFrame.numberOfRecords)
            {
                _currentRecordNumber = 0;
                _currentElementType = ELEMENTTYPE_NONE;
            }

        }

    }
    else
    {
        // ???
    }
    

    return result;
}


bool IldaStreamReader::IsDataEnd()
{
    return _rawDataPosition >= _rawDataSize;
}


void IldaStreamReader::Reset()
{

    memset((void *)_currentRecord, 0, RECORD_BUFFER_SIZE);

    memset((void *)&_currentFrame, 0, sizeof(IldaFrame));
    _currentFrame.numberOfRecords = -1;
    _currentFrame.frameNumber = -1;
    _currentFrame.totalFrames = -1;

    _currentRecordNumber = 0;

    _currentElementPosition = 0;
    _currentElementType = ELEMENTTYPE_NONE;
    _isCurrentElementRead = false;
    _lastReadElementType = ELEMENTTYPE_NONE;

    _rawDataPosition = 0;

}

int IldaStreamReader::SetRawData(const uint8_t * inRawData, int rawDataSize)
{

    _rawData = inRawData;
    _rawDataSize = rawDataSize;
    _rawDataPosition = 0;

    return ERROR_SUCCESS;

}