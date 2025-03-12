#include "../D2/BitStream.h"
#include <cmath>       // para std::log2, std::ceil
#include <stdexcept>   // para exceções
#include "Golomb.h"

Golomb::Golomb(int mVal) : m(mVal) {
    if (m <= 0) {
        throw std::invalid_argument("Golomb parameter m must be > 0");
    }
}

// (x >= 0) ? (2*x) : (-2*x - 1)
uint64_t Golomb::zigzagEncode(int64_t x) const {
    if (x >= 0) {
        return static_cast<uint64_t>(x) << 1;  // 2*x
    } else {
        // -2*x - 1
        // cuidado ao converter para uint64_t
        // mas em C++ a expressão -2*x - 1 é segura se x for int64_t
        return static_cast<uint64_t>((-x << 1) - 1);
    }
}

int64_t Golomb::zigzagDecode(uint64_t v) const {
    // Se v é par => v/2, se v é ímpar => -( (v+1)/2 )
    if ((v & 1) == 0) {
        // par
        return static_cast<int64_t>(v >> 1);
    } else {
        // ímpar
        return -static_cast<int64_t>((v + 1) >> 1);
    }
}

void Golomb::encode(int64_t x, BitStream &bs) {
    // 1) Converte x para não-negativo via zigzag
    uint64_t xMapped = zigzagEncode(x);

    // 2) Divide por m: q e r
    uint64_t q = xMapped / m;
    uint64_t r = xMapped % m;

    // 3) Escreve q em unário (q vezes '1', depois '0')
    for (uint64_t i = 0; i < q; ++i) {
        bs.writeBit(true);  // bit '1'
    }
    bs.writeBit(false);     // bit '0'

    // 4) Escreve r em "truncated binary"
    //    b = ceil(log2(m)), cutoff = 2^b - m
    int b = static_cast<int>(std::ceil(std::log2(m)));
    uint64_t cutoff = (static_cast<uint64_t>(1) << b) - m;

    if (r < cutoff) {
        // r cabe em (b-1) bits
        bs.writeBits(r, b - 1);
    } else {
        // caso contrário, grava (r + cutoff) em b bits
        bs.writeBits(r + cutoff, b);
    }
}

int64_t Golomb::decode(BitStream &bs) {
    // 1) Lê parte unária para descobrir q
    uint64_t q = 0;
    while (true) {
        bool bit;
        try {
            bit = bs.readBit();
        } catch (...) {
            throw std::runtime_error("Golomb decode: EOF ao ler parte unária.");
        }
        if (bit) {
            // se for '1'
            q++;
        } else {
            // se for '0', terminou a parte unária
            break;
        }
    }

    // 2) Lê resto em "truncated binary"
    int b = static_cast<int>(std::ceil(std::log2(m)));
    uint64_t cutoff = (static_cast<uint64_t>(1) << b) - m;

    uint64_t rCandidate = 0;
    try {
        rCandidate = bs.readBits(b - 1);
    } catch (...) {
        throw std::runtime_error("Golomb decode: EOF ao ler resto (b-1 bits).");
    }

    uint64_t r;
    if (rCandidate < cutoff) {
        r = rCandidate;
    } else {
        // Precisamos ler 1 bit adicional
        uint64_t extraBit = 0;
        try {
            extraBit = bs.readBits(1);
        } catch (...) {
            throw std::runtime_error("Golomb decode: EOF ao ler bit extra.");
        }
        // fullValue = (rCandidate << 1) + extraBit (total b bits)
        uint64_t fullValue = (rCandidate << 1) | extraBit;
        r = fullValue + cutoff;
    }

    // 3) xMapped = q*m + r
    uint64_t xMapped = q * static_cast<uint64_t>(m) + r;

    // 4) Inverso do zigzag
    return zigzagDecode(xMapped);
}
