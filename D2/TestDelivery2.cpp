#include "BitStream.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <cassert>

//   g++ TestDelivery2.cpp BitStream.cpp -o TestDelivery2
//   ./TestDelivery2

using namespace std;
using namespace std::chrono;

void testBits(const std::string& filename) {
    cout << "\n--- Teste: Escrita e Leitura de Bits ---" << endl;
    vector<bool> bits = {1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0};
    // Escrita
    auto t_write_start = high_resolution_clock::now();
    {
        BitStream bs(filename, true);
        for (bool b : bits) bs.writeBit(b);
    }
    auto t_write_end = high_resolution_clock::now();
    cout << "Tempo de escrita de bits: " << duration_cast<microseconds>(t_write_end - t_write_start).count() << " us" << endl;
    // Leitura
    auto t_read_start = high_resolution_clock::now();
    vector<bool> lidos;
    {
        BitStream bs(filename, false);
        for (size_t i = 0; i < bits.size(); ++i) lidos.push_back(bs.readBit());
    }
    auto t_read_end = high_resolution_clock::now();
    cout << "Tempo de leitura de bits: " << duration_cast<microseconds>(t_read_end - t_read_start).count() << " us" << endl;
    // Validação
    assert(bits == lidos);
    cout << "Bits lidos coincidem com os escritos." << endl;
}

void testIntegers(const std::string& filename) {
    cout << "\n--- Teste: Escrita e Leitura de Inteiros ---" << endl;
    vector<uint64_t> valores = {5, 255, 1023, 42};
    vector<int> bitsPorValor = {3, 8, 10, 6};
    // Escrita
    {
        BitStream bs(filename, true);
        for (size_t i = 0; i < valores.size(); ++i)
            bs.writeBits(valores[i], bitsPorValor[i]);
    }
    // Leitura
    vector<uint64_t> lidos;
    {
        BitStream bs(filename, false);
        for (size_t i = 0; i < valores.size(); ++i)
            lidos.push_back(bs.readBits(bitsPorValor[i]));
    }
    // Validação
    assert(valores == lidos);
    cout << "Inteiros lidos coincidem com os escritos." << endl;
}

void testStrings(const std::string& filename) {
    cout << "\n--- Teste: Escrita e Leitura de Strings ---" << endl;
    string texto = "CSLP2024";
    // Escrita
    {
        BitStream bs(filename, true);
        bs.writeString(texto);
    }
    // Leitura
    string lido;
    {
        BitStream bs(filename, false);
        lido = bs.readString(texto.size());
    }
    // Validação
    assert(texto == lido);
    cout << "String lida coincide com a escrita." << endl;
}

void testAlignAndEOF(const std::string& filename) {
    cout << "\n--- Teste: Alinhamento e EOF ---" << endl;
    // Escreve bits e depois alinha
    {
        BitStream bs(filename, true);
        bs.writeBit(1);
        bs.writeBit(0);
        bs.alignToByte();
        bs.writeBits(7, 3); // 3 bits
    }
    // Lê bits, alinha, lê mais
    {
        BitStream bs(filename, false);
        bool b1 = bs.readBit();
        bool b2 = bs.readBit();
        bs.alignToByte();
        uint64_t v = bs.readBits(3);
        assert(b1 == 1 && b2 == 0 && v == 7);
        cout << "Alinhamento e leitura após alinhamento corretos." << endl;
        // Teste de EOF
        try {
            bs.readBit();
            cout << "[ERRO] Não lançou exceção ao ler após EOF!" << endl;
        } catch (const std::exception& e) {
            cout << "Exceção ao ler após EOF (esperado): " << e.what() << endl;
        }
    }
}

int main() {
    cout << "==== Teste Delivery 2: BitStream ====" << endl;
    auto t_start = chrono::high_resolution_clock::now();
    testBits("test_bits.bin");
    testIntegers("test_ints.bin");
    testStrings("test_str.bin");
    testAlignAndEOF("test_align.bin");
    auto t_end = chrono::high_resolution_clock::now();
    cout << "\nTempo total de execução: " << chrono::duration_cast<chrono::milliseconds>(t_end - t_start).count() << " ms" << endl;
    cout << "==== Fim dos Testes ====" << endl;
    return 0;
}
