// #include "IldaEsp32Output.h"

// void IldaEsp32Output::Init()
// {
//     pinMode(_pinBlanking, OUTPUT);
    
//     SetAndOut(0, 0, 0, 0, 0, 0, true);
// }


// void IldaEsp32Output::SetCoords(int16_t x, int16_t y, int16_t z)
// {
//     _x = x;
//     _y = y;
//     _z = z;
// }

// void IldaEsp32Output::SetColor(uint8_t r, uint8_t g, uint8_t b)
// {
//     _r = r;
//     _g = g;
//     _b = b;
// }

// void IldaEsp32Output::SetBlank(bool value)
// {
//     _blanking = value;
// }


// void IldaEsp32Output::SetAndOut(
//     int16_t x, int16_t y, int16_t z, 
//     uint8_t r, uint8_t g, uint8_t b, 
//     bool blank)
// {
//     _x = x;
//     _y = y;
//     _z = z;

//     _r = r;
//     _g = g;
//     _b = b;

//     _blanking = blank;
    
//     Out();
// }

// void IldaEsp32Output::Out()
// {
//     // esp wroom32
    
//     digitalWrite(_pinBlanking, _blanking ? _pinBlankingEnable : _pinBlankingDisable);

//     dacWrite(_pinDacX, (_x + 32768) / 256);
//     dacWrite(_pinDacY, (_y + 32768) / 256);
// }