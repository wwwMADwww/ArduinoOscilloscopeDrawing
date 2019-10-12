
#include "IldaStreamReader.h"

#include "IldaProcessor.h"

// #include "IldaSDCardDataReader.h"
#include "IldaSerialDataReader.h"

// #include "IldaEsp32Output.h"
#include "IldaAtmega32u4BufOutput.h"


IldaProcessor * processor;

void setup()
{

    //IldaSDCardDataReader * reader = new IldaSDCardDataReader();

    Serial.begin(115200);
    Serial.setTimeout(0);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    

    IldaSerialDataReader * reader = new IldaSerialDataReader(&Serial);

    // IldaEsp32Output * out = new IldaEsp32Output();
    IldaAtmega32u4BufOutput * out = new IldaAtmega32u4BufOutput();

    IldaStreamReader * streamreader = new IldaStreamReader();

    processor = new IldaProcessor(reader, out, streamreader);

    processor->Init();

}

void loop()
{
    int res = processor->Process();

    if ((res == IldaStreamReader::ERROR_INVALID_RECORD_FORMAT) ||
        ((res & 0x100) > 0))
    {
        Serial.print("error ");
        Serial.println(res);
    }
    
    // Serial.print("res ");
    // Serial.print(res);
    // Serial.print(", code ");
    // Serial.println(processor->_code);

    // delay(250);

}
