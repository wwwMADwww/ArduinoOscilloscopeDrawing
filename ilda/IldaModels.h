#ifndef ILDAMODELS_H

#define ILDAMODELS_H

// https://www.ilda.com/resources/StandardsDocs/ILDA_IDTF14_rev011.pdf

const int ILDA_RECORD_STATUS_FLAG_BLANKING  = 0b01000000;
const int ILDA_RECORD_STATUS_FLAG_LASTPOINT = 0b10000000;

struct IldaFrame
{
  char ILDA[4];             // 1-4 "ILDA"
  
  uint8_t reserved_05;      // 5
  uint8_t reserved_06;      // 6
  uint8_t reserved_07;      // 7

  uint8_t formatCode;       // 8
  
  char frameName[8];        // 9-16
  char companyName[8];      // 17-24
  
  uint16_t numberOfRecords; // 25-26
  
  uint16_t frameNumber;     // 27-28
  uint16_t totalFrames;     // 29-30
    
  uint8_t projectorNumber;  // 31
  
  uint8_t reserved_32;      // 32
};

struct IldaRecordF0
{
  int16_t x;          // 1-2
  int16_t y;          // 3-4
  int16_t z;          // 5-6
  
  uint8_t statusCode; // 7
  
  uint8_t colorIndex; // 8
};

struct IldaRecordF1
{
  int16_t x;          // 1-2
  int16_t y;          // 3-4
  
  uint8_t statusCode; // 5
  
  uint8_t colorIndex; // 6
};

struct IldaRecordF2
{
  uint8_t r; // 1
  uint8_t g; // 2
  uint8_t b; // 3
};

struct IldaRecordF4
{
  int16_t x;          // 1-2
  int16_t y;          // 3-4
  int16_t z;          // 5-6
  
  uint8_t statusCode; // 7
  
  uint8_t r;          // 8
  uint8_t g;          // 9
  uint8_t b;          // 10
};

struct IldaRecordF5
{
  int16_t x;          // 1-2
  int16_t y;          // 3-4
  
  uint8_t statusCode; // 5
  
  uint8_t r;          // 6
  uint8_t g;          // 7
  uint8_t b;          // 8
};



#endif