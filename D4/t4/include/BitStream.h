#pragma once
#include <fstream>
#include <string>

// g++ D3/GolombTest.cpp D3/Golomb.cpp D2/BitStream.cpp -o D3/GolombTest
// ./D3/GolombTest 5 D3/input.txt D3/encoded.bin

class BitStream {
public:
    BitStream(const std::string& filename, bool writeMode);
    ~BitStream();

    /* 1 bit */
    void writeBit(bool bit);
    bool readBit();

    /* 1 byte */
    void writeByte(unsigned char byte);
    unsigned char readByte();

    /* 32-bit little-endian */
    void writeInt(int value);
    int  readInt();

    /* n bits ( 0 < n ≤ 32 ) */
    void writeBits(unsigned int value, int nBits);
    unsigned int readBits(int nBits);

    /* descarrega buffer parcial (apenas quando em escrita) */
    void flush();

private:
    /* ficheiros */
    std::ofstream ofs;
    std::ifstream ifs;
    bool writeMode{false};

    /* escrita */
    unsigned char buffer{0};
    int  bitCount{0};

    /* leitura */
    unsigned char readBuffer{0};
    int  bitsLeft{0};
};
