#pragma once

#include <fstream>
#include <string>

class BitStream {
public:
    // Construtor: filename e modo (true = write, false = read)
    BitStream(const std::string& filename, bool writeMode);

    ~BitStream();

    // Escrita e leitura de bits
    void writeBit(bool bit);
    bool readBit();

    void writeBits(uint64_t value, int nbits);
    uint64_t readBits(int nbits);

    // Escrever e ler strings como sequência de bits
    void writeString(const std::string& str);
    std::string readString(size_t length);

    void flush();

private:
    std::ofstream ofs;
    std::ifstream ifs;

    bool writeMode;

    unsigned char buffer;
    int bitPos; // posição no buffer (0-7)

    void flushBuffer();
};
