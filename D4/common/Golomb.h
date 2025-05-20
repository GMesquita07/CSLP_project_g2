#ifndef GOLOMB_H
#define GOLOMB_H

#include <cstdint>


class Golomb {
private:
    int m;  // Parâmetro Golomb

    // Mapeia int64_t (positivo ou negativo) para uint64_t não negativo
    // (zigzag): 0->0, -1->1, 1->2, -2->3, 2->4, ...
    uint64_t zigzagEncode(int64_t x) const;

    // Inverso do zigzag: recupera o valor original (positivo/negativo)
    int64_t zigzagDecode(uint64_t v) const;

public:
    // Construtor que recebe o parâmetro 'm'
    explicit Golomb(int mVal);

    // Codifica o inteiro x (pode ser negativo) e escreve no BitStream
    void encode(int64_t x, BitStream &bs);

    // Lê do BitStream e decodifica, retornando o inteiro original
    int64_t decode(BitStream &bs);
};

#endif // GOLOMB_H
