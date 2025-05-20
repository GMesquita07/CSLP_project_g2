#include "BitStream.h"

BitStream::BitStream(const std::string& filename, bool writeMode) : writeMode(writeMode), buffer(0), bitCount(0) {
    if (writeMode) {
        ofs.open(filename, std::ios::binary);
    } else {
        ifs.open(filename, std::ios::binary);
    }
}

BitStream::~BitStream() {
    if (writeMode) {
        flush();
        ofs.close();
    } else {
        ifs.close();
    }
}

void BitStream::writeBit(bool bit) {
    buffer = (buffer << 1) | bit;
    bitCount++;

    if (bitCount == 8) {
        ofs.put(buffer);
        bitCount = 0;
        buffer = 0;
    }
}

bool BitStream::readBit() {
    static unsigned char readBuffer = 0;
    static int bitsLeft = 0;

    if (bitsLeft == 0) {
        readBuffer = ifs.get();
        bitsLeft = 8;
    }
    bool bit = (readBuffer & (1 << (bitsLeft - 1))) != 0;
    bitsLeft--;
    return bit;
}

void BitStream::writeByte(unsigned char byte) {
    if (bitCount == 0) {
        ofs.put(byte);
    } else {
        // escrever byte bit a bit se buffer parcialmente cheio
        for (int i = 7; i >= 0; --i) {
            writeBit((byte >> i) & 1);
        }
    }
}

unsigned char BitStream::readByte() {
    unsigned char byte = 0;
    for (int i = 7; i >= 0; --i) {
        bool bit = readBit();
        byte |= (bit << i);
    }
    return byte;
}

void BitStream::writeInt(int value) {
    // escreve 4 bytes (32 bits) little endian
    for (int i = 0; i < 4; ++i) {
        unsigned char byte = (value >> (8 * i)) & 0xFF;
        writeByte(byte);
    }
}

int BitStream::readInt() {
    int value = 0;
    for (int i = 0; i < 4; ++i) {
        unsigned char byte = readByte();
        value |= (byte << (8 * i));
    }
    return value;
}

void BitStream::flush() {
    if (bitCount > 0) {
        buffer <<= (8 - bitCount);
        ofs.put(buffer);
        bitCount = 0;
        buffer = 0;
    }
}

void BitStream::writeBits(unsigned int value, int nBits) {
    for (int i = nBits - 1; i >= 0; --i) {
        bool bit = (value >> i) & 1;
        writeBit(bit);
    }
}

unsigned int BitStream::readBits(int nBits) {
    unsigned int value = 0;
    for (int i = 0; i < nBits; ++i) {
        bool bit = readBit();
        value = (value << 1) | bit;
    }
    return value;
}

