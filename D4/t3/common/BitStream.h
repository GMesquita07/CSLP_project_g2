// BitStream.h
#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdint>

// Classe BitStream para leitura/escrita de bits em arquivo.
// Agora conta com métodos adicionais de alinhamento e verificação de EOF.
class BitStream {
private:
    std::fstream file;       // stream do arquivo
    unsigned char buffer;    // buffer de 1 byte para armazenar bits intermediários
    int bitCount;            // quantos bits validos existem em 'buffer'
    bool isWriting;          // indica se estamos em modo escrita ou leitura

    // Escreve qualquer sobra do buffer no arquivo (modo escrita).
    void flushBuffer();

public:
    // Abre o arquivo no modo indicado (true = write, false = read)
    BitStream(const std::string &filename, bool writeMode);

    // Fecha o arquivo e faz flush (se necessario)
    ~BitStream();

    // Escreve um unico bit (0 ou 1) no arquivo
    void writeBit(bool bit);

    // Le um unico bit (retorna true/false) do arquivo
    bool readBit();

    // Escreve "numBits" bits do valor "value" no arquivo.
    // 0 < numBits < 64.
    void writeBits(uint64_t value, int numBits);

    // Le "numBits" bits do arquivo e retorna num valor uint64_t.
    // 0 < numBits < 64.
    uint64_t readBits(int numBits);

    // Escreve cada caracter da string (8 bits) usando writeBits()
    void writeString(const std::string &data);

    // Le "length" caracteres do arquivo (8 bits cada) usando readBits()
    std::string readString(size_t length);

    // Alinha no byte seguinte (descarta bits pendentes no buffer ao ler;
    // flush pendente ao escrever)
    void alignToByte();

    // Retorna true se chegar ao EOF em modo leitura (considerando buffer esgotado)
    bool eof();

    // Fecha o arquivo manualmente (chamado tbm no destrutor)
    void close();
};

#endif