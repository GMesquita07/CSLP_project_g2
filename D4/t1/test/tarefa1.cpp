#include "predictive_codec.h"
#include "predictor.h"
#include "metrics.h"
#include <opencv2/opencv.hpp>
#include <chrono>
#include <sys/stat.h>
#include <iostream>

//    cd D4/t1/build
//    cmake ..
//    make

//    g++ ../test/tarefa1.cpp ../src/predictive_codec.cpp ../src/predictor.cpp ../src/metrics.cpp ../src/Golomb.cpp ../src/BitStream.cpp -I../include `pkg-config --cflags --libs opencv4` -o tarefa1
//    ./tarefa1

size_t getFileSize(const std::string& filename) {
    struct stat st;
    if (stat(filename.c_str(), &st) == 0)
        return st.st_size;
    return 0;
}

int main() {
    std::string inputImage = "../data/lebron.webp";
    std::string encodedFile = "encoded_lossless.bin";

    cv::Mat img = cv::imread(inputImage, cv::IMREAD_COLOR);
    if (img.empty()) {
        std::cerr << "Erro ao carregar imagem." << std::endl;
        return -1;
    }

    int m = 8; // parâmetro Golomb
    auto t_enc_start = std::chrono::high_resolution_clock::now();
    encodeImageLossless(img, encodedFile, m);
    auto t_enc_end = std::chrono::high_resolution_clock::now();
    std::cout << "Imagem codificada com sucesso.\n";

    auto t_dec_start = std::chrono::high_resolution_clock::now();
    cv::Mat decoded = decodeImageLossless(encodedFile, img.rows, img.cols, m);
    auto t_dec_end = std::chrono::high_resolution_clock::now();
    std::cout << "Imagem descodificada com sucesso.\n";

    double mse = calculateMSE(img, decoded);
    double psnr = calculatePSNR(mse);

    size_t inputSize = getFileSize(inputImage);
    size_t encodedSize = getFileSize(encodedFile);
    double compressao = inputSize > 0 ? 100.0 * (1.0 - (double)encodedSize / inputSize) : 0.0;

    std::cout << "==== Métricas Delivery 4 - T1 ====" << std::endl;
    std::cout << "Tamanho original: " << inputSize << " bytes" << std::endl;
    std::cout << "Tamanho codificado: " << encodedSize << " bytes" << std::endl;
    std::cout << "Compressão: " << compressao << "%" << std::endl;
    std::cout << "MSE: " << mse << std::endl;
    std::cout << "PSNR: " << psnr << " dB" << std::endl;
    std::cout << "Tempo de codificação: " << std::chrono::duration_cast<std::chrono::milliseconds>(t_enc_end - t_enc_start).count() << " ms" << std::endl;
    std::cout << "Tempo de descodificação: " << std::chrono::duration_cast<std::chrono::milliseconds>(t_dec_end - t_dec_start).count() << " ms" << std::endl;
    std::cout << "=============================" << std::endl;

    cv::imshow("Original", img);
    cv::imshow("Reconstruída", decoded);
    cv::waitKey(0);
    return 0;
}