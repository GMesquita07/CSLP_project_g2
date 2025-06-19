#include "video_codec.h"   
#include <iostream>
#include <chrono>
#include <sys/stat.h>

//   cd D4/t3/build
//   cmake ..
//   make
//   ./delivery3

size_t getFileSize(const std::string& filename) {
    struct stat st;
    if (stat(filename.c_str(), &st) == 0)
        return st.st_size;
    return 0;
}

int main() {
    std::string inputVideo = "../data/testvideo.mp4";              // para testar com video original, mudar testvideo -> testvideo15seg  (EXTREMAMENTE MAIS DEMORADO)
    std::string encodedFile = "encoded_interframe.bin";
    std::string outputVideo = "decoded_interframe.avi";

    int m = 8;              // parâmetro Golomb
    int blockSize = 8;      // tamanho do bloco para compensação de movimento
    int searchArea = 4;     // área de busca ±4 pixels
    int IFramePeriod = 10;

    auto t_enc_start = std::chrono::high_resolution_clock::now();
    std::cout << "A codificar vídeo com codificação inter-frame...\n";
    encodeVideoInterFrame(inputVideo, encodedFile, m, blockSize, searchArea, IFramePeriod);
    auto t_enc_end = std::chrono::high_resolution_clock::now();
    std::cout << "Vídeo codificado com sucesso.\n";

    auto t_dec_start = std::chrono::high_resolution_clock::now();
    std::cout << "A descodificar vídeo...\n";
    decodeVideoInterFrame(encodedFile, outputVideo);
    auto t_dec_end = std::chrono::high_resolution_clock::now();
    std::cout << "Vídeo descodificado com sucesso.\n";

    size_t inputSize = getFileSize(inputVideo);
    size_t encodedSize = getFileSize(encodedFile);
    size_t outputSize = getFileSize(outputVideo);
    double compressao = inputSize > 0 ? 100.0 * (1.0 - (double)encodedSize / inputSize) : 0.0;

    std::cout << "==== Métricas Delivery 4 - T3 ====" << std::endl;
    std::cout << "Tamanho original: " << inputSize << " bytes" << std::endl;
    std::cout << "Tamanho codificado: " << encodedSize << " bytes" << std::endl;
    std::cout << "Tamanho reconstruído: " << outputSize << " bytes" << std::endl;
    std::cout << "Compressão: " << compressao << "%" << std::endl;
    std::cout << "Tempo de codificação: " << std::chrono::duration_cast<std::chrono::milliseconds>(t_enc_end - t_enc_start).count() << " ms" << std::endl;
    std::cout << "Tempo de descodificação: " << std::chrono::duration_cast<std::chrono::milliseconds>(t_dec_end - t_dec_start).count() << " ms" << std::endl;
    std::cout << "=============================" << std::endl;

    // (Opcional) Se tiveres funções para calcular PSNR/frame, podes adicionar aqui
    // Exemplo:
    // double psnr = calculateVideoPSNR(inputVideo, outputVideo);
    // std::cout << "PSNR médio: " << psnr << " dB" << std::endl;

    return 0;
}
