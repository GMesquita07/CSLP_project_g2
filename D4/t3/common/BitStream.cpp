// BitStream.cpp
#include "BitStream.h"
#include <iostream>

BitStream::BitStream(const std::string &filename, bool writeMode)
    : buffer(0), bitCount(0), isWriting(writeMode)
{
    // Abre arquivo em modo binario (escrita ou leitura)
    if (writeMode)
        file.open(filename, std::ios::out | std::ios::binary);
    else
        file.open(filename, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("BitStream: Unable to open file " + filename);
    }
}

BitStream::~BitStream() {
    // Garante flush e fecha ao destruir
    close();
}

void BitStream::writeBit(bool bit) {
    if (!isWriting) {
        throw std::runtime_error("BitStream: Attempting to write in read mode.");
    }

    // Desloca o buffer para a esquerda e adiciona o novo bit no LSB
    buffer = (buffer << 1) | (bit ? 1 : 0);
    bitCount++;

    // Se completamos um byte, flush no arquivo
    if (bitCount == 8) {
        flushBuffer();
    }
}

bool BitStream::readBit() {
    if (isWriting) {
        throw std::runtime_error("BitStream: Attempting to read in write mode.");
    }

    // Se nao ha bits disponiveis no buffer, ler proximo byte do arquivo
    if (bitCount == 0) {
        if (!file.read(reinterpret_cast<char*>(&buffer), 1)) {
            // Se falhou leitura, possivel EOF
            throw std::runtime_error("BitStream: Attempt to read past EOF.");
        }
        bitCount = 8;
    }

    // Extrai o bit mais significativo do buffer
    bool bit = (buffer & (1 << (bitCount - 1))) != 0;
    bitCount--;

    return bit;
}

void BitStream::writeBits(uint64_t value, int numBits) {
    if (numBits <= 0 || numBits >= 64)
        throw std::runtime_error("BitStream: numBits out of range (writeBits)");

    // Escreve bit a bit, do mais significativo pro menos
    for (int i = numBits - 1; i >= 0; --i) {
        bool b = (value & (1ULL << i)) != 0ULL;
        writeBit(b);
    }
}

uint64_t BitStream::readBits(int numBits) {
    if (numBits <= 0 || numBits >= 64)
        throw std::runtime_error("BitStream: numBits out of range (readBits)");

    uint64_t value = 0;
    for (int i = 0; i < numBits; ++i) {
        bool b = readBit();
        value = (value << 1) | (b ? 1ULL : 0ULL);
    }
    return value;
}

void BitStream::writeString(const std::string &data) {
    for (unsigned char c : data) {
        writeBits(static_cast<uint64_t>(c), 8);
    }
}

std::string BitStream::readString(size_t length) {
    std::string result;
    result.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        unsigned char c = static_cast<unsigned char>(readBits(8));
        result.push_back(static_cast<char>(c));
    }
    return result;
}

void BitStream::alignToByte() {
    if (isWriting) {
        // Se tem bits pendentes, flushBuffer completando com zeros
        flushBuffer();
    } else {
        // Descartar bits remanescentes no buffer em modo leitura
        bitCount = 0;
    }
}

bool BitStream::eof() {
    if (isWriting) {
        // Em modo escrita, EOF nao faz sentido
        return false;
    }
    // Se file.eof() ja e true e nao temos bits em buffer pra ler, entao eof
    if (file.eof()) {
        return (bitCount == 0);
    }
    // Caso nao sejamos explicitamente eof, mas sem bits lidos no buffer
    // so e possivel saber ao tentar ler. Portanto, retornamos false.
    return false;
}

void BitStream::close() {
    if (file.is_open()) {
        // Se ainda estiver em modo escrita, flush final
        if (isWriting) {
            flushBuffer();
        }
        file.close();
    }
}

void BitStream::flushBuffer() {
    if (!isWriting) {
        return; // sem acao em modo leitura
    }

    if (bitCount > 0) {
        // Completa com zeros a esquerda se nao tinha 8 bits ainda
        buffer <<= (8 - bitCount);
        file.write(reinterpret_cast<char*>(&buffer), 1);
        buffer = 0;
        bitCount = 0;
    }
}