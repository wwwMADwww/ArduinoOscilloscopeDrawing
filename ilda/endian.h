#ifndef ENDIAN_H

#define ENDIAN_H

// https://codereview.stackexchange.com/questions/151049/endianness-conversion-in-c

#define REVERSE_16BIT(n) ((int16_t) (((n & 0x00FF) << 8) | \
                                     ((n & 0xFF00) >> 8)))

#define REVERSE_U16BIT(n) ((uint16_t) REVERSE_16BIT(n) )

#define REVERSE_32BIT(n) ((int32_t) (((n & 0x000000FF) << 24) | \
                                     ((n & 0x0000FF00) <<  8) | \
                                     ((n & 0x00FF0000) >>  8) | \
                                     ((n & 0xFF000000) >> 24)))

#define REVERSE_U32BIT(n) ((uint32_t) REVERSE_32BIT(n))


#endif