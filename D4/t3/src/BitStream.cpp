#include "BitStream.h"

/* ───────── ctor / dtor ───────── */
BitStream::BitStream(const std::string& filename, bool writeMode)
    : writeMode(writeMode), buffer(0), bitCount(0),
      readBuffer(0), bitsLeft(0)
{
    if (writeMode)
        ofs.open(filename, std::ios::binary);
    else
        ifs.open(filename, std::ios::binary);
}

BitStream::~BitStream() {
    if (writeMode) { flush(); ofs.close(); }
    else            ifs.close();
}

/* ───────── bit a bit ───────── */
void BitStream::writeBit(bool bit) {
    buffer = (buffer << 1) | bit;
    if (++bitCount == 8) { ofs.put(buffer); buffer = 0; bitCount = 0; }
}

bool BitStream::readBit() {
    if (bitsLeft == 0) {                 // carrega novo byte
        readBuffer = ifs.get();
        bitsLeft   = 8;
    }
    bool bit = (readBuffer & (1 << (bitsLeft - 1))) != 0;
    --bitsLeft;
    return bit;
}

/* ───────── byte ───────── */
void BitStream::writeByte(unsigned char byte) {
    if (bitCount == 0) ofs.put(byte);           // alinhado
    else for (int i = 7; i >= 0; --i) writeBit((byte >> i) & 1);
}

unsigned char BitStream::readByte() {
    unsigned char byte = 0;
    for (int i = 7; i >= 0; --i) byte |= (readBit() << i);
    return byte;
}

/* ───────── 32 bits ───────── */
void BitStream::writeInt(int value) {
    for (int i = 0; i < 4; ++i) writeByte( (value >> (8*i)) & 0xFF );
}

int BitStream::readInt() {
    int value = 0;
    for (int i = 0; i < 4; ++i) value |= (readByte() << (8*i));
    return value;
}

/* ───────── n bits genérico ───────── */
void BitStream::writeBits(unsigned int value, int nBits) {
    for (int i = nBits-1; i >= 0; --i) writeBit( (value >> i) & 1 );
}

unsigned int BitStream::readBits(int nBits) {
    unsigned int v = 0;
    for (int i = 0; i < nBits; ++i) v = (v << 1) | readBit();
    return v;
}

/* ───────── flush ───────── */
void BitStream::flush() {
    if (bitCount > 0) {                  // completa byte com zeros
        buffer <<= (8 - bitCount);
        ofs.put(buffer);
        buffer = 0; bitCount = 0;
    }
}
