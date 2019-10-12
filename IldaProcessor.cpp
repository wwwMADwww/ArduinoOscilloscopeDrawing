#include "IldaProcessor.h"


IldaProcessor::IldaProcessor(
    IldaDataReaderBase * ildaDataReader, 
    IldaOutputBase * ildaOutput,
    IldaStreamReader * ildaStreamReader)
{

    _ildaDataReader = ildaDataReader;
    _ildaOutput = ildaOutput;
    
    _ildaStreamReader = ildaStreamReader;

}


void IldaProcessor::Init()
{

    Reset();

    _ildaDataReader->Init();

    _ildaOutput->Init();

    pinMode(  5, OUTPUT);

}


void IldaProcessor::Reset()
{
    if (_databuf) delete _databuf;
    if (_recordBuf) delete _recordBuf;
    if (_colorPalette) delete _colorPalette;

    _nodatamillisStart = 0;
    _nodatamillisCurrent = 0;

    _code = IldaStreamReader::ERROR_NOT_ENOUGHT_DATA;

    _x = 0;
    _y = 0;
    _z = 0;

    _r = 0;
    _g = 0;
    _b = 0;

    _blanking = true;
    _lastPoint = false;

    _dataBufSize = 1024; //64; //760; // 256; //1024 * 10;
    _databuf = new uint8_t[_dataBufSize];

    _recordBuf = new uint8_t[IldaStreamReader::RECORD_BUFFER_SIZE];

    _colorPalette = new IldaRecordF2[0];

    _colorPaletteReadID = 0;

    
    _ildaOutput->NoDataLed(true);

}



int IldaProcessor::Process()
{
    // Serial.println("Processing Ilda file");

    if (_code == IldaStreamReader::ERROR_NOT_ENOUGHT_DATA)
    {
        _ildaOutput->SetBlank(true);
        _ildaOutput->Out();

        int bytesRead = _ildaDataReader->Read(_databuf, _dataBufSize);

        if (bytesRead < 0)
        {
            _nodatamillisCurrent = millis();

            if (_nodatamillisStart == 0)
                _nodatamillisStart = _nodatamillisCurrent;
            else if (_nodatamillisCurrent - _nodatamillisStart > 100)
            {
                _ildaStreamReader->Reset();
                _ildaOutput->NoDataLed(true);
                Reset();
            }
            return _code;
        }
        else
        {
            _nodatamillisStart = 0;
            _nodatamillisCurrent = 0;
            _ildaOutput->NoDataLed(false);
        }

        _code = _ildaStreamReader->SetRawData(_databuf, bytesRead);
    }
    
    _code = _ildaStreamReader->Read();

    if (_code == IldaStreamReader::ERROR_NOT_ENOUGHT_DATA)
    {
        return _code;
    }

    if (_code == IldaStreamReader::ERROR_INVALID_RECORD_FORMAT)
    {
        return _code;
    }

    int elementType = IldaStreamReader::ELEMENTTYPE_NONE;
    _code = _ildaStreamReader->GetLastReadElementType(&elementType);
    
    if (_code == IldaStreamReader::ERROR_NOT_ENOUGHT_DATA)
    {
        return _code;
    }

    if (elementType == IldaStreamReader::ELEMENTTYPE_FRAME)
    {
        _code = _ildaStreamReader->GetFrame(&_frame);

        if (_code == IldaStreamReader::ERROR_NOT_ENOUGHT_DATA)
        {
            return _code;
        }

        if (_frame.numberOfRecords == 0)
        {
            _ildaDataReader->ResetToBegin();
            _code = IldaStreamReader::ERROR_NOT_ENOUGHT_DATA;
            return _code;
        }

        if (_frame.formatCode == 2)
        {
            delete _colorPalette;

            _r = 0;
            _g = 0;
            _b = 0;
            _blanking = true;

            _colorPalette = new IldaRecordF2[_frame.numberOfRecords];
            memset((void *) _colorPalette, 0, _frame.numberOfRecords * sizeof(IldaRecordF2));
        }
    }
    else if (elementType == IldaStreamReader::ELEMENTTYPE_RECORD)
    {

        _code = _ildaStreamReader->GetRecord(_recordBuf);

        if (_code == IldaStreamReader::ERROR_NOT_ENOUGHT_DATA)
        {
            return _code;
        }

        switch (_frame.formatCode)
        {
            case 0: 
            {
                IldaRecordF0 * record = (IldaRecordF0 *) _recordBuf;

                _blanking  = (record->statusCode & ILDA_RECORD_STATUS_FLAG_BLANKING) > 0;
                _lastPoint = (record->statusCode & ILDA_RECORD_STATUS_FLAG_LASTPOINT) > 0;

                _x = record->x;
                _y = record->y;
                _z = record->z;

                _r = _colorPalette[record->colorIndex].r;
                _g = _colorPalette[record->colorIndex].g;
                _b = _colorPalette[record->colorIndex].b;

                break;
            }
            case 1: 
            {
                IldaRecordF1 * record = (IldaRecordF1 *) _recordBuf;

                _blanking  = (record->statusCode & ILDA_RECORD_STATUS_FLAG_BLANKING) > 0;
                _lastPoint = (record->statusCode & ILDA_RECORD_STATUS_FLAG_LASTPOINT) > 0;

                _x = record->x;
                _y = record->y;

                _r = _colorPalette[record->colorIndex].r;
                _g = _colorPalette[record->colorIndex].g;
                _b = _colorPalette[record->colorIndex].b;

                break;
            }
            case 2: 
            {
                IldaRecordF2 * record = (IldaRecordF2 *) _recordBuf;

                _colorPalette[_colorPaletteReadID].r = record->r;
                _colorPalette[_colorPaletteReadID].g = record->g;
                _colorPalette[_colorPaletteReadID].b = record->b;

                _colorPaletteReadID++;

                break;
            }
            case 4:
            {
                IldaRecordF4 * record = (IldaRecordF4 *) _recordBuf;

                _blanking  = (record->statusCode & ILDA_RECORD_STATUS_FLAG_BLANKING) > 0;
                _lastPoint = (record->statusCode & ILDA_RECORD_STATUS_FLAG_LASTPOINT) > 0;

                _x = record->x;
                _y = record->y;
                _z = record->z;

                _r = record->r;
                _g = record->g;
                _b = record->b;

                break;
            }
            case 5:
            {
                IldaRecordF5 * record = (IldaRecordF5 *) _recordBuf;

                _blanking  = (record->statusCode & ILDA_RECORD_STATUS_FLAG_BLANKING) > 0;
                _lastPoint = (record->statusCode & ILDA_RECORD_STATUS_FLAG_LASTPOINT) > 0;

                _x = record->x;
                _y = record->y;

                _r = record->r;
                _g = record->g;
                _b = record->b;

                break;
            }

            default:
            {   
                // Serial.print("Unknown record format: ");
                // Serial.println(_frame.formatCode);
                // digitalWrite(5, HIGH);
                return 0x102;
            }
        }
    }
    else
    {
        // Serial.print("Unknown error code: ");
        // Serial.println(_code);
        
        // digitalWrite(5, HIGH);
        return 0x103;
    }

    // _x = _testx++;

    _ildaOutput->SetAndOut(
        _x, 
        _y, 
        _z, 

        _r,
        _g, 
        _b, 

        _blanking
    );
    

    // digitalWrite(5, HIGH);

    return _code;

}

