#include "IldaStreamReader.h"


IldaStreamReader::IldaStreamReader()
{
    _currentRecord = (uint8_t *) std::malloc(RECORD_BUFFER_SIZE);
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

int IldaStreamReader::GetRecord(void * outIldaRecord)
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

        if ((copySize + _rawDataPosition) <= _rawDataLength)
        {
            _isCurrentElementRead = true;
            _lastReadElementType = _currentElementType;
        }
        else
        {
            copySize = _rawDataLength - _rawDataPosition;
            result = ERROR_NOT_ENOUGHT_DATA;
            _isCurrentElementRead = false;
        }

        memcpy(((uint8_t *)&_currentFrame + _currentElementPosition), ((uint8_t *)_rawData + _rawDataPosition), copySize);

        _currentElementPosition = (_currentElementPosition + copySize) % frameSize;
        _rawDataPosition = _rawDataPosition + copySize;

        if (_isCurrentElementRead && _currentFrame.totalFrames > 0)
        {
            _currentFrame.numberOfRecords = REVERSE_U16BIT(_currentFrame.numberOfRecords);
            _currentFrame.frameNumber = REVERSE_U16BIT(_currentFrame.frameNumber);
            _currentFrame.totalFrames = REVERSE_U16BIT(_currentFrame.totalFrames);

            _currentElementType = ELEMENTTYPE_RECORD;
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
                break;
        }

        int copySize = recordSize - _currentElementPosition;

        if ((copySize + _rawDataPosition) <= _rawDataLength)
        {
            _isCurrentElementRead = true;
            _lastReadElementType = _currentElementType;
            _currentRecordNumber++;
        }
        else
        {
            copySize = _rawDataLength - _rawDataPosition;
            result = ERROR_NOT_ENOUGHT_DATA;
            _isCurrentElementRead = false;
        }

        memcpy((_currentRecord + _currentElementPosition), ((uint8_t *)_rawData + _rawDataPosition), recordSize);
        
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
    else
    {
        // ???
    }
    

    return result;
}


bool IldaStreamReader::IsDataEnd()
{
    return _rawDataPosition >= _rawDataLength;
}


void IldaStreamReader::Reset()
{

    memset((void *)_currentRecord, 0, RECORD_BUFFER_SIZE);

    memset(&_currentFrame, 0, sizeof(IldaFrame));
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

int IldaStreamReader::SetRawData(const void * inRawData, int lenght)
{

    _rawData = inRawData;
    _rawDataLength = lenght;
    _rawDataPosition = 0;

    return ERROR_SUCCESS;

}