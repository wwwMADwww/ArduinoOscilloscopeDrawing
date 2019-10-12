#include "IldaSerialDataReader.h"


// IldaSerialDataReader::IldaSerialDataReader(HardwareSerial * stream)
IldaSerialDataReader::IldaSerialDataReader(Serial_ * stream)
{
    _stream = stream;
}


void IldaSerialDataReader::Init()
{
    

}


int IldaSerialDataReader::Read(uint8_t * buf, int bufSize)
{

    memset(buf, 0, bufSize);

    if (_stream->available() == 0)
        return -1;

    int read = _stream->readBytes(buf, bufSize);

    return read;

}


void IldaSerialDataReader::ResetToBegin()
{
    _stream->write('A');
}