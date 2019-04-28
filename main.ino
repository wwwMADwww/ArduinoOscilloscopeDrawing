
#include "IldaStreamReader.h"

#include "FS.h"
#include "SD.h"
#include "SPI.h"

const int pinBlanking = 33;
const int pinBlankingEnable = HIGH;
const int pinBlankingDisable = LOW;

const int pinDacX = 25;
const int pinDacY = 26;

const int sdCardWaitTime = 3000;

const int fileReadBufSize = 1024 * 10;

int filenameLen = 255;
char * filename;

File ildaFile;
int ildaFileSize = 0;


struct SystemState {

    int16_t x;
    int16_t y;
    int16_t z;

    int16_t r;
    int16_t g;
    int16_t b;

    bool blanking;
    bool lastPoint;
};

SystemState systemState;

void systemStateReset()
{
    systemState.x = 0;
    systemState.y = 0;
    systemState.z = 0;
    systemState.r = 0;
    systemState.g = 0;
    systemState.b = 0;
    systemState.blanking = true;
    systemState.lastPoint = false;
}

void sdCardLoad()
{

    while(true)
    {
        
        Serial.println("Waiting for SD Card...");

        ildaFile.close();

        if(!SD.begin())
        {
            Serial.println("Card Mount Failed");
            // return;
            delay(sdCardWaitTime);
            continue;
        }
        uint8_t cardType = SD.cardType();

        if(cardType == CARD_NONE)
        {
            Serial.println("No SD card attached");
            // return;
            continue;
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
            Serial.println("Failed to open /ILDA/filename.txt for reading");
            // return;
            continue;
        }
        
        int filenamepos = 0;
        while(file.available() && filenamepos < filenameLen)
        {
            filename[filenamepos++] = file.read();
        }
        file.close();

        Serial.printf("ilda filename is '%s'\n", filename);


        ildaFile = SD.open(filename);
        
        if(!ildaFile)
        {
            Serial.println("Failed to open ilda file for reading");
            // return;
            continue;
        }

        
        ildaFileSize = ildaFile.size();

        Serial.print("File size ");
        Serial.println(ildaFileSize);

        break;

    } // /while

}


void laserOutput() 
{
    // esp wroom32
    
    // if ((systemState.r == 0 && systemState.g == 0 && systemState.b == 0) ||
    //     systemState.z == -32767)
    //     systemState.blanking = true;

    digitalWrite(pinBlanking, systemState.blanking ? pinBlankingEnable : pinBlankingDisable);

    dacWrite(pinDacX, (systemState.x + 32768) / 256);
    dacWrite(pinDacY, (systemState.y + 32768) / 256);
}


void processildaFile()
{

    Serial.println("Processing Ilda file");

    int code = IldaStreamReader::ERROR_NOT_ENOUGHT_DATA;
    uint8_t * filebuf = new uint8_t[fileReadBufSize];

    IldaStreamReader reader;
    IldaFrame frame;
    uint8_t * recordBuf = new uint8_t[IldaStreamReader::RECORD_BUFFER_SIZE];

    IldaRecordF2 * colorPalette = new IldaRecordF2[0];

    int colorPaletteReadID = 0;
    bool fileWasRead = false;

    while (true)
    {

        if (code == IldaStreamReader::ERROR_NOT_ENOUGHT_DATA)
        {
            int bytesRead = 0;

            if (!fileWasRead || ildaFileSize > fileReadBufSize)
            {

                systemState.blanking = true;
                laserOutput();

                if (!ildaFile.available())
                {
                    if (!ildaFile.seek(0))
                    {
                        break;
                    }
                }

                bytesRead = ildaFile.read(filebuf, fileReadBufSize);
                if (bytesRead <= 0)
                {
                    break;
                }

                fileWasRead = true;

            }
            else
            {
                bytesRead = ildaFileSize;
            }

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
                ildaFile.seek(0);
                code = IldaStreamReader::ERROR_NOT_ENOUGHT_DATA;
                continue;
            }

            if (frame.formatCode == 2)
            {
                delete colorPalette;

                systemState.r = 0;
                systemState.g = 0;
                systemState.b = 0;
                systemState.blanking = true;

                colorPalette = new IldaRecordF2[frame.numberOfRecords];
                memset((void *) colorPalette, 0, frame.numberOfRecords * sizeof(IldaRecordF2));
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

                    systemState.blanking = (record->statusCode & ILDA_RECORD_STATUS_FLAG_BLANKING) > 0;
                    systemState.lastPoint = (record->statusCode & ILDA_RECORD_STATUS_FLAG_LASTPOINT) > 0;

                    systemState.x = record->x;
                    systemState.y = record->y;
                    systemState.z = record->z;

                    systemState.r = colorPalette[record->colorIndex].r;
                    systemState.g = colorPalette[record->colorIndex].g;
                    systemState.b = colorPalette[record->colorIndex].b;

                    break;
                }
                case 1: 
                {
                    IldaRecordF1 * record = (IldaRecordF1 *) recordBuf;

                    systemState.blanking = (record->statusCode & ILDA_RECORD_STATUS_FLAG_BLANKING) > 0;
                    systemState.lastPoint = (record->statusCode & ILDA_RECORD_STATUS_FLAG_LASTPOINT) > 0;

                    systemState.x = record->x;
                    systemState.y = record->y;

                    systemState.r = colorPalette[record->colorIndex].r;
                    systemState.g = colorPalette[record->colorIndex].g;
                    systemState.b = colorPalette[record->colorIndex].b;

                    break;
                }
                case 2: 
                {
                    IldaRecordF2 * record = (IldaRecordF2 *) recordBuf;

                    colorPalette[colorPaletteReadID].r = record->r;
                    colorPalette[colorPaletteReadID].g = record->g;
                    colorPalette[colorPaletteReadID].b = record->b;

                    colorPaletteReadID++;

                    break;
                }
                case 4:
                {
                    IldaRecordF4 * record = (IldaRecordF4 *) recordBuf;

                    systemState.blanking = (record->statusCode & ILDA_RECORD_STATUS_FLAG_BLANKING) > 0;
                    systemState.lastPoint = (record->statusCode & ILDA_RECORD_STATUS_FLAG_LASTPOINT) > 0;

                    systemState.x = record->x;
                    systemState.y = record->y;
                    systemState.z = record->z;

                    systemState.r = record->r;
                    systemState.g = record->g;
                    systemState.b = record->b;

                    break;
                }
                case 5:
                {
                    IldaRecordF5 * record = (IldaRecordF5 *) recordBuf;

                    systemState.blanking = (record->statusCode & ILDA_RECORD_STATUS_FLAG_BLANKING) > 0;
                    systemState.lastPoint = (record->statusCode & ILDA_RECORD_STATUS_FLAG_LASTPOINT) > 0;

                    systemState.x = record->x;
                    systemState.y = record->y;

                    systemState.r = record->r;
                    systemState.g = record->g;
                    systemState.b = record->b;

                    break;
                }

                default:
                {   
                    Serial.print("Unknown record format: ");
                    Serial.println(frame.formatCode);
                    break;
                }
            }
        }
        else
        {
            Serial.print("Unknown error code: ");
            Serial.println(code);
            break;
        }

        laserOutput();
    
    } // /while


    delete filebuf;
    delete recordBuf;
    delete colorPalette;

}

void setup()
{

    systemStateReset();

    pinMode(pinBlanking, OUTPUT);


    Serial.begin(115200);

    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }

}

void loop()
{
    systemStateReset();
    sdCardLoad();
    processildaFile();
}
