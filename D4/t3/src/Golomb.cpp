#include "Golomb.h"
#include <cmath>

GolombEncoder::GolombEncoder(BitStream& bs_, int m_)
    : bs(bs_), m(m_) 
{
    b = static_cast<int>(std::ceil(std::log2(m)));
    threshold = (1 << b) - m;
}

void GolombEncoder::encodeUnsigned(unsigned int value) {
    unsigned int q = value / m;
    int r = value % m;

    // Codificar quociente em unary (q zeros + 1)
    for (unsigned int i = 0; i < q; ++i) bs.writeBit(0);
    bs.writeBit(1);

    // Codificar resto
    encodeRemainder(r);
}

void GolombEncoder::flush() {
    if (bitCount > 0) {
        buffer <<= (32 - bitCount); // completar os bits restantes
        bs.writeBits(buffer, bitCount);
        bitCount = 0;
        buffer = 0;
    }
}

void GolombEncoder::encodeSigned(int value) {
    // Mapeamento Zig-Zag
    unsigned int mapped = (value <= 0) ? (-value) * 2 : value * 2 - 1;
    encodeUnsigned(mapped);
}

void GolombEncoder::encodeRemainder(int remainder) {
    if (remainder < threshold) {
        bs.writeBits(remainder, b -1);
    } else {
        remainder += threshold;
        bs.writeBits(remainder, b);
    }
}

GolombDecoder::GolombDecoder(BitStream& bs_, int m_)
    : bs(bs_), m(m_)
{
    b = static_cast<int>(std::ceil(std::log2(m)));
    threshold = (1 << b) - m;
}

unsigned int GolombDecoder::decodeUnsigned() {
    unsigned int q = 0;
    while (!bs.readBit()) ++q;

    int r = decodeRemainder();

    return q * m + r;
}

int GolombDecoder::decodeSigned() {
    unsigned int mapped = decodeUnsigned();
    int value = (mapped % 2 == 0) ? -(static_cast<int>(mapped) / 2) : (static_cast<int>(mapped) + 1) / 2;
    return value;
}

int GolombDecoder::decodeRemainder() {
    int r = static_cast<int>(bs.readBits(b -1));
    if (r >= threshold) {
        int extra = static_cast<int>(bs.readBit());
        r = (r << 1) + extra - threshold;
    }
    return r;
}
