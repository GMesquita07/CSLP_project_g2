#include "video_codec.h"
#include <iostream>
#include <chrono>
#include <sys/stat.h>

// g++ ../test/tarefa2.cpp \
    ../src/video_codec.cpp \
    ../src/BitStream.cpp \
    ../src/Golomb.cpp \
    ../src/predictive_codec.cpp \
    ../src/predictor.cpp \
    ../src/metrics.cpp \
    -I../include -I../../t1/include \
    `pkg-config --cflags --libs opencv4` -o tarefa2


// ./tarefa2 

size_t getFileSize(const std::string& filename) {
    struct stat st;
    if (stat(filename.c_str(), &st) == 0)
        return st.st_size;
    return 0;
}

int main() {
    // Define o caminho do vídeo de entrada - muda para o teu vídeo real
    std::string inputVideo = "../data/testvideo.mp4";

    // Ficheiro onde vai ficar o vídeo codificado
    std::string encodedFile = "encoded_video.bin";

    // Ficheiro para o vídeo descodificado
    std::string outputVideo = "decoded_output.avi";

    int m = 8;  // parâmetro Golomb

    auto t_enc_start = std::chrono::high_resolution_clock::now();
    std::cout << "A codificar vídeo...\n";
    encodeVideoLossless(inputVideo, encodedFile, m);
    auto t_enc_end = std::chrono::high_resolution_clock::now();
    std::cout << "Vídeo codificado com sucesso.\n";

    auto t_dec_start = std::chrono::high_resolution_clock::now();
    std::cout << "A descodificar vídeo...\n";
    decodeVideoLossless(encodedFile, outputVideo);
    auto t_dec_end = std::chrono::high_resolution_clock::now();
    std::cout << "Vídeo descodificado com sucesso.\n";

    size_t inputSize = getFileSize(inputVideo);
    size_t encodedSize = getFileSize(encodedFile);
    size_t outputSize = getFileSize(outputVideo);
    double compressao = inputSize > 0 ? 100.0 * (1.0 - (double)encodedSize / inputSize) : 0.0;

    std::cout << "==== Métricas Delivery 4 - T2 ====" << std::endl;
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
