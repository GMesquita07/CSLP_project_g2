#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <cstdint>
#include <fstream>

class BitStream {
public:
    BitStream(const std::string& filename, bool isOutput);
    ~BitStream();

    void writeBit(bool bit);
    bool readBit();
    void writeByte(uint8_t byte);
    uint8_t readByte();
    void writeBits(const uint8_t* bits, size_t numBits);
    void readBits(uint8_t* bits, size_t numBits);
    void flush();

private:
    std::fstream file;
    uint8_t buffer;
    size_t bitCount;
};

#endif 