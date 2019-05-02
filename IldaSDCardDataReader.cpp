#include "IldaSDCardDataReader.h"

void IldaSDCardDataReader::Init()
{
    
    while(true)
    {
        
        // Serial.println("Waiting for SD Card...");

        _file.close();

        if(!SD.begin())
        {
            // Serial.println("Card Mount Failed");
            // return;
            delay(_sdCardWaitTime);
            continue;
        }
        uint8_t cardType = SD.cardType();

        if(cardType == CARD_NONE)
        {
            // Serial.println("No SD card attached");
            // return;
            continue;
        }

        // Serial.print("SD Card Type: ");
        if(cardType == CARD_MMC){
            // Serial.println("MMC");
        } else if(cardType == CARD_SD){
            // Serial.println("SDSC");
        } else if(cardType == CARD_SDHC){
            // Serial.println("SDHC");
        } else {
            // Serial.println("UNKNOWN");
        }

        uint64_t cardSize = SD.cardSize() / (1024 * 1024);
        // Serial.printf("SD Card Size: %lluMB\n", cardSize);

        
        _filename = new char[_filenameLen];
        memset(_filename, 0, _filenameLen);

        
        File file = SD.open("/ILDA/filename.txt");
        if(!file)
        {
            // Serial.println("Failed to open /ILDA/filename.txt for reading");
            // return;
            continue;
        }
        
        int filenamepos = 0;
        while(file.available() && filenamepos < _filenameLen)
        {
            _filename[filenamepos++] = file.read();
        }
        file.close();

        // Serial.printf("ilda filename is '%s'\n", _filename);


        _file = SD.open(_filename);
        
        if(!_file)
        {
            // Serial.println("Failed to open ilda file for reading");
            // return;
            continue;
        }

        
        // ildaFileSize = _file.size();

        // Serial.print("File size ");
        // Serial.println(ildaFileSize);

        break;

    } // /while

}


int IldaSDCardDataReader::Read(uint8_t * buf, int bufSize)
{
    int bytesRead = 0;

    if (!_file.available())
    {
        // if (!_file.seek(0))
        // {
        //     return 0;
        // }

        return 0;
    }

    bytesRead = _file.read(buf, bufSize);

    return bytesRead;
}


void IldaSDCardDataReader::ResetToBegin()
{
    _file.seek(0);
}