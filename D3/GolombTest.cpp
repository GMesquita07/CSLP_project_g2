#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdint>
#include "../D2/BitStream.h" // sua classe de T1
#include "Golomb.h"    // a implementação Golomb.h e Golomb.cpp

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <m> <input.txt> <encoded.bin>\n";
        return 1;
    }

    // 1) Lê parâmetros
    int mVal = std::stoi(argv[1]);
    std::string inputFile = argv[2];
    std::string encodedFile = argv[3];

    // 2) Lê inteiros do arquivo texto
    std::ifstream fin(inputFile);
    if (!fin) {
        std::cerr << "Cannot open input file " << inputFile << "\n";
        return 1;
    }

    std::vector<int64_t> originalValues;
    int64_t val;
    while (fin >> val) {
        originalValues.push_back(val);
    }
    fin.close();

    // 3) Codifica (Encoder)
    {
        // Abre arquivo binário para escrita
        BitStream bs(encodedFile, true); // true = write mode
        Golomb golomb(mVal);

        // Para cada inteiro, faz encode
        for (auto x : originalValues) {
            golomb.encode(x, bs);
        }
        // Ao sair do escopo, o destrutor do BitStream faz flush e fecha o arquivo
    }

    // 4) Decodifica (Decoder)
    std::vector<int64_t> decodedValues;
    {
        // Abre arquivo binário para leitura
        BitStream bs(encodedFile, false);
        Golomb golomb(mVal);

        // Decodifica a mesma quantidade de valores
        for (size_t i = 0; i < originalValues.size(); i++) {
            try {
                int64_t decoded = golomb.decode(bs);
                decodedValues.push_back(decoded);
            } catch (std::exception &e) {
                std::cerr << "Error decoding index " << i 
                          << ": " << e.what() << "\n";
                break;
            }
        }
    }

    // 5) Compara e exibe resultado
    size_t mismatchCount = 0;
    for (size_t i = 0; i < originalValues.size(); i++) {
        if (originalValues[i] != decodedValues[i]) {
            mismatchCount++;
            std::cerr << "Mismatch at index " << i << ": original=" 
                      << originalValues[i] << ", decoded=" 
                      << decodedValues[i] << "\n";
        }
    }

    if (mismatchCount == 0) {
        std::cout << "All " << originalValues.size() 
                  << " values matched successfully!\n";
    } else {
        std::cout << "Found " << mismatchCount 
                  << " mismatches out of " << originalValues.size() << " total.\n";
    }

    return 0;
}
