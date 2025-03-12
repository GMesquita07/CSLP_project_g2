#include "../D2/BitStream.h"
#include <cmath>       // para std::log2, std::floor
#include <stdexcept>   // para exceções
#include "Golomb.h"

Golomb::Golomb(int mVal) : m(mVal) {
    if (m <= 0) {
        throw std::invalid_argument("Golomb parameter m must be > 0");
    }
}

// Zigzag: converte int64_t (pos/neg) em uint64_t não negativo
uint64_t Golomb::zigzagEncode(int64_t x) const {
    // x >= 0 => 2*x, x < 0 => -2*x - 1
    if (x >= 0) {
        return static_cast<uint64_t>(x) << 1;  // 2*x
    } else {
        return static_cast<uint64_t>((-x << 1) - 1);
    }
}

// Inverso do zigzag: converte uint64_t de volta para int64_t original
int64_t Golomb::zigzagDecode(uint64_t v) const {
    // se v for par => v/2, se v for ímpar => -( (v+1)/2 )
    if ((v & 1) == 0) {
        // par
        return static_cast<int64_t>(v >> 1);
    } else {
        // ímpar
        return -static_cast<int64_t>((v + 1) >> 1);
    }
}

void Golomb::encode(int64_t x, BitStream &bs) {
    // 1) Mapeia x para xMapped >= 0 via zigzag
    uint64_t xMapped = zigzagEncode(x);

    // 2) q = xMapped / m, r = xMapped % m
    uint64_t q = xMapped / m;
    uint64_t r = xMapped % m;

    // 3) Codifica q em unário => q vezes '1' + '0'
    for (uint64_t i = 0; i < q; ++i) {
        bs.writeBit(true);  // bit '1'
    }
    bs.writeBit(false);     // bit '0'

    // 4) Codificação do resto em "truncated binary"
    //    b = floor(log2(m))
    //    leftover = 2^(b+1) - m
    int b = static_cast<int>(std::floor(std::log2(m)));
    uint64_t leftover = (static_cast<uint64_t>(1) << (b + 1)) - m;

    if (r < leftover) {
        // Se r < leftover, grava r em b bits
        bs.writeBits(r, b);
    } else {
        // Se não, grava (r + leftover) em (b+1) bits
        bs.writeBits(r + leftover, b + 1);
    }
}

int64_t Golomb::decode(BitStream &bs) {
    // 1) Ler unário para obter q
    uint64_t q = 0;
    while (true) {
        bool bit;
        try {
            bit = bs.readBit();
        } catch (...) {
            throw std::runtime_error("Golomb decode: EOF ao ler parte unária.");
        }
        if (bit) {
            // '1'
            q++;
        } else {
            // '0' => fim da parte unária
            break;
        }
    }

    // 2) Ler resto em "truncated binary"
    int b = static_cast<int>(std::floor(std::log2(m)));
    uint64_t leftover = (static_cast<uint64_t>(1) << (b + 1)) - m;

    // Ler b bits iniciais
    uint64_t val = 0;
    try {
        val = bs.readBits(b);
    } catch (...) {
        throw std::runtime_error("Golomb decode: EOF ao ler resto (b bits).");
    }

    uint64_t r;
    if (val < leftover) {
        // se val < leftover, r = val
        r = val;
    } else {
        // senão, precisamos ler 1 bit adicional para total (b+1) bits
        uint64_t extraBit = 0;
        try {
            extraBit = bs.readBits(1);
        } catch (...) {
            throw std::runtime_error("Golomb decode: EOF ao ler bit extra.");
        }
        // fullValue = (val << 1) | extraBit
        uint64_t fullValue = (val << 1) | extraBit;
        r = fullValue - leftover;
    }

    // 3) xMapped = q*m + r
    uint64_t xMapped = q * static_cast<uint64_t>(m) + r;

    // 4) Inverso do zigzag => x original
    return zigzagDecode(xMapped);
}
