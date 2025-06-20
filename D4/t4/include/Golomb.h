#pragma once

#include "BitStream.h"

class GolombEncoder {
public:
    GolombEncoder(BitStream& bs, int m);
    void encodeUnsigned(unsigned int value);
    void encodeSigned(int value);
    void flush();

private:
    BitStream& bs;
    int m;
    int b; // número de bits para o resto
    int threshold;

    void encodeRemainder(int remainder);
};

class GolombDecoder {
public:
    GolombDecoder(BitStream& bs, int m);
    unsigned int decodeUnsigned();
    int decodeSigned();

private:
    BitStream& bs;
    int m;
    int b;
    int threshold;

    int decodeRemainder();
};
