#include "BitStream.h"
#include <stdexcept>

BitStream::BitStream(const std::string& filename, bool writeMode)
    : writeMode(writeMode), buffer(0), bitPos(0)
{
    if (writeMode) {
        ofs.open(filename, std::ios::binary);
        if (!ofs.is_open()) throw std::runtime_error("Failed to open file for writing");
    } else {
        ifs.open(filename, std::ios::binary);
        if (!ifs.is_open()) throw std::runtime_error("Failed to open file for reading");
    }
}

BitStream::~BitStream() {
    if (writeMode) flushBuffer();
    if (ofs.is_open()) ofs.close();
    if (ifs.is_open()) ifs.close();
}

void BitStream::writeBit(bool bit) {
    buffer <<= 1;
    if (bit) buffer |= 1;
    bitPos++;
    if (bitPos == 8) flushBuffer();
}

bool BitStream::readBit() {
    if (bitPos == 0) {
        if (!ifs.get(reinterpret_cast<char&>(buffer))) throw std::runtime_error("End of file");
        bitPos = 8;
    }
    bool bit = (buffer & 0x80) != 0;
    buffer <<= 1;
    bitPos--;
    return bit;
}

void BitStream::writeBits(uint64_t value, int nbits) {
    if (nbits <= 0 || nbits > 64) throw std::invalid_argument("Invalid nbits");
    for (int i = nbits -1; i >=0; --i) {
        writeBit((value >> i) & 1);
    }
}

uint64_t BitStream::readBits(int nbits) {
    if (nbits <= 0 || nbits > 64) throw std::invalid_argument("Invalid nbits");
    uint64_t value = 0;
    for (int i = 0; i < nbits; ++i) {
        value <<= 1;
        value |= readBit() ? 1 : 0;
    }
    return value;
}

void BitStream::writeString(const std::string& str) {
    for (char c : str) {
        writeBits(static_cast<uint8_t>(c), 8);
    }
}

std::string BitStream::readString(size_t length) {
    std::string str;
    for (size_t i = 0; i < length; ++i) {
        char c = static_cast<char>(readBits(8));
        str.push_back(c);
    }
    return str;
}

void BitStream::flushBuffer() {
    if (bitPos == 0) return;
    buffer <<= (8 - bitPos);
    ofs.put(buffer);
    buffer = 0;
    bitPos = 0;
}

void BitStream::flush() {
    flushBuffer();
    ofs.flush();
}
