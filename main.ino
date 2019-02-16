
#include "IldaStreamReader.h"

#include "FS.h"
#include "SD.h"
#include "SPI.h"

const int pinBlanking = 33;
const int pinBlankingEnable = HIGH;
const int pinBlankingDisable = LOW;

const int pinDacX = 25;
const int pinDacY = 26;

const int fileReadBufSize = 1024 * 10;

int filenameLen = 255;
char * filename;

File ildafile;

void setup()
{

    pinMode(pinBlanking, OUTPUT);

    Serial.begin(115200);

    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }

    Serial.begin(115200);
    if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    
    filename = new char[filenameLen];
    memset(filename, 0, filenameLen);

    
    File file = SD.open("/ILDA/filename.txt");
    if(!file)
    {
        Serial.println("Failed to open filename.txt for reading");
        return;
    }    
    
    int filenamepos = 0;
    while(file.available() && filenamepos < filenameLen)
    {
        filename[filenamepos++] = file.read();
    }
    file.close();

    Serial.printf("ilda filename is '%s'\n", filename);


    ildafile = SD.open(filename);
    
    if(!ildafile)
    {
        Serial.println("Failed to open ilda file for reading");
        return;
    }

}


void loop()
{

    int code = IldaStreamReader::ERROR_NOT_ENOUGHT_DATA;
    uint8_t * filebuf = new uint8_t[fileReadBufSize];

    IldaStreamReader reader;
    IldaFrame frame;
    uint8_t * recordBuf = new uint8_t[IldaStreamReader::RECORD_BUFFER_SIZE];

    int16_t x = 0, y = 0;
    bool blanking = true;
    bool lastPoint = false;

    while (true)
    {



        if (code == IldaStreamReader::ERROR_NOT_ENOUGHT_DATA)
        {

            digitalWrite(pinBlanking, pinBlankingEnable);

            if (!ildafile.available())
            {
                ildafile.seek(0);
            }

            int bytesRead = ildafile.read(filebuf, fileReadBufSize);

            code = reader.SetRawData(filebuf, bytesRead);
        }
        
        code = reader.Read();

        if (code == IldaStreamReader::ERROR_NOT_ENOUGHT_DATA)
        {
            continue;
        }

        if (code == IldaStreamReader::ERROR_INVALID_RECORD_FORMAT)
        {
            break;
        }

        int elementType = IldaStreamReader::ELEMENTTYPE_NONE;
        code = reader.GetLastReadElementType(&elementType);
        
        if (code == IldaStreamReader::ERROR_NOT_ENOUGHT_DATA)
        {
            continue;
        }
        if (elementType == IldaStreamReader::ELEMENTTYPE_FRAME)
        {
            code = reader.GetFrame(&frame);

            if (code == IldaStreamReader::ERROR_NOT_ENOUGHT_DATA)
            {
                continue;
            }

            if (frame.numberOfRecords == 0)
            {
                break;
            }
        }
        else if (elementType == IldaStreamReader::ELEMENTTYPE_RECORD)
        {

            code = reader.GetRecord(recordBuf);

            if (code == IldaStreamReader::ERROR_NOT_ENOUGHT_DATA)
            {
                continue;
            }

            switch (frame.formatCode)
            {
                case 0: 
                {
                    IldaRecordF0 * record = (IldaRecordF0 *) recordBuf;

                    blanking = (record->statusCode & ILDA_RECORD_STATUS_FLAG_BLANKING) > 0;
                    lastPoint = (record->statusCode & ILDA_RECORD_STATUS_FLAG_LASTPOINT) > 0;

                    x = record->x;
                    y = record->y;
                    break;
                }
                case 1: 
                {
                    IldaRecordF1 * record = (IldaRecordF1 *) recordBuf;

                    blanking = (record->statusCode & ILDA_RECORD_STATUS_FLAG_BLANKING) > 0;
                    lastPoint = (record->statusCode & ILDA_RECORD_STATUS_FLAG_LASTPOINT) > 0;

                    x = record->x;
                    y = record->y;
                    break;
                }
                case 2: 
                {
                    IldaRecordF2 * record = (IldaRecordF2 *) recordBuf;
                    break;
                }
                case 4:
                {
                    IldaRecordF4 * record = (IldaRecordF4 *) recordBuf;

                    blanking = (record->statusCode & ILDA_RECORD_STATUS_FLAG_BLANKING) > 0;
                    lastPoint = (record->statusCode & ILDA_RECORD_STATUS_FLAG_LASTPOINT) > 0;

                    x = record->x;
                    y = record->y;
                    break;
                }
                case 5:
                {
                    IldaRecordF5 * record = (IldaRecordF5 *) recordBuf;

                    blanking = (record->statusCode & ILDA_RECORD_STATUS_FLAG_BLANKING) > 0;
                    lastPoint = (record->statusCode & ILDA_RECORD_STATUS_FLAG_LASTPOINT) > 0;

                    x = record->x;
                    y = record->y;

                    break;
                }

                default:
                    break;
            }
        }
        else
        {
            break;
        }

        digitalWrite(pinBlanking, blanking ? pinBlankingEnable : pinBlankingDisable);

        dacWrite(pinDacX, (x + 32768) / 256);
        dacWrite(pinDacY, (y + 32768) / 256);
    
    } // /while


    delete filebuf;
    delete recordBuf;

}

