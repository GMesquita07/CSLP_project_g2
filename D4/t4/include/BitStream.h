#pragma once

#include <fstream>
#include <string>

class BitStream {
public:
    BitStream(const std::string& filename, bool writeMode);
    ~BitStream();

    void writeBit(bool bit);
    bool readBit();

    // Escreve 8 bits (um byte)
    void writeByte(unsigned char byte);
    // Lê 8 bits (um byte)
    unsigned char readByte();

    // NOVO: Escrever e ler inteiros de 32 bits
    void writeInt(int value);
    int readInt();

    void flush();
    unsigned int readBits(int nBits);
    void writeBits(unsigned int value, int nBits);


private:
    std::ofstream ofs;
    std::ifstream ifs;

    bool writeMode;

    unsigned char buffer;
    int bitCount;

    void writeBuffer();
};
