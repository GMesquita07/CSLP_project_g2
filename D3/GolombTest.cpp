#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdint>
#include <chrono>
#include <sys/stat.h>
#include "../D2/BitStream.h" 
#include "Golomb.h"    

// g++ GolombTest.cpp Golomb.cpp ../D2/BitStream.cpp -o GolombTest
// ./GolombTest <m> input.txt encoded.bin

// Função para obter o tamanho de um ficheiro
size_t getFileSize(const std::string& filename) {
    struct stat st;
    if (stat(filename.c_str(), &st) == 0)
        return st.st_size;
    return 0;
}

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

    // 2) Codifica (Encoder)
    auto t_encode_start = std::chrono::high_resolution_clock::now();
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
    auto t_encode_end = std::chrono::high_resolution_clock::now();

    // 3) Decodifica (Decoder)
    std::vector<int64_t> decodedValues;
    auto t_decode_start = std::chrono::high_resolution_clock::now();
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
    auto t_decode_end = std::chrono::high_resolution_clock::now();

    // 4) Compara e exibe resultado
    size_t mismatchCount = 0;
    for (size_t i = 0; i < originalValues.size(); i++) {
        if (originalValues[i] != decodedValues[i]) {
            mismatchCount++;
            std::cerr << "Mismatch at index " << i << ": original=" 
                      << originalValues[i] << ", decoded=" 
                      << decodedValues[i] << "\n";
        }
    }

    // 5) Métricas
    size_t fileOriginal = getFileSize(inputFile); // ficheiro de input (texto)
    size_t inputSize = originalValues.size() * sizeof(int64_t); // RAW (binário)
    size_t encodedSize = getFileSize(encodedFile);
    double compressao = inputSize > 0 ? 100.0 * (1.0 - (double)encodedSize / inputSize) : 0.0;
    auto encode_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t_encode_end - t_encode_start).count();
    auto decode_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t_decode_end - t_decode_start).count();
    auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t_decode_end - t_encode_start).count();

    std::cout << "\n==== Métricas Golomb ====" << std::endl;
    std::cout << "Tamanho ficheiro de input (texto): " << fileOriginal << " bytes" << std::endl;
    std::cout << "Tamanho RAW (binário): " << inputSize << " bytes" << std::endl;
    std::cout << "Tamanho do ficheiro codificado: " << encodedSize << " bytes" << std::endl;
    std::cout << "Compressão (vs RAW): " << compressao << "%" << std::endl;
    std::cout << "Tempo de codificação: " << encode_ms << " ms" << std::endl;
    std::cout << "Tempo de descodificação: " << decode_ms << " ms" << std::endl;
    std::cout << "Tempo total (codificação + descodificação): " << total_ms << " ms" << std::endl;

    if (mismatchCount == 0) {
        std::cout << "Todos os " << originalValues.size() 
                  << " valores coincidem após codificação/descodificação!" << std::endl;
    } else {
        std::cout << "Encontrados " << mismatchCount 
                  << " erros em " << originalValues.size() << " valores." << std::endl;
    }
    std::cout << "========================\n";
    return 0;
}
