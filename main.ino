
#include "IldaStreamReader.h"
#include "IldaProcessor.h"
#include "IldaSDCardDataReader.h"
#include "IldaEsp32Output.h"


IldaProcessor * processor;

void setup()
{

    IldaSDCardDataReader * reader = new IldaSDCardDataReader();

    IldaEsp32Output * out = new IldaEsp32Output();

    IldaStreamReader * streamreader = new IldaStreamReader();

    processor = new IldaProcessor(reader, out, streamreader);

    processor->Init();

}

void loop()
{
    processor->Process();
}
