#pragma once
#include <fstream>
#include <string>

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
