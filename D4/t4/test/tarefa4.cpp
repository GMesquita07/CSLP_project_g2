#include "lossy_codec.h"
#include "metrics.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <sys/stat.h>

//    cd D4/t4/build
//    cmake ..
//    make
//    ./delivery4

size_t getFileSize(const std::string& filename) {
    struct stat st;
    if (stat(filename.c_str(), &st) == 0)
        return st.st_size;
    return 0;
}

int main() {
    std::string inputImage = "../data/lebron.webp";
    std::string encodedFile = "encoded_lossy.bin";
    std::string encodedFileDCT = "encoded_dct.bin";

    int m = 8;
    int quantLevel = 10;
    int blockSize = 8;

    cv::Mat img = cv::imread(inputImage, cv::IMREAD_COLOR);
    if (img.empty()) {
        std::cerr << "Erro ao carregar imagem." << std::endl;
        return -1;
    }
    size_t inputSize = getFileSize(inputImage);

    // --- Quantização simples ---
    auto t_enc1_start = std::chrono::high_resolution_clock::now();
    encodeImageLossy(img, encodedFile, m, quantLevel);
    auto t_enc1_end = std::chrono::high_resolution_clock::now();
    auto t_dec1_start = std::chrono::high_resolution_clock::now();
    cv::Mat decodedLossy = decodeImageLossy(encodedFile, img.rows, img.cols, m, quantLevel);
    auto t_dec1_end = std::chrono::high_resolution_clock::now();
    double mse1 = calculateMSE(img, decodedLossy);
    double psnr1 = calculatePSNR(mse1);
    size_t encodedSize1 = getFileSize(encodedFile);
    double compressao1 = inputSize > 0 ? 100.0 * (1.0 - (double)encodedSize1 / inputSize) : 0.0;

    // --- DCT + ZigZag + Quantização ---
    auto t_enc2_start = std::chrono::high_resolution_clock::now();
    encodeImageDCT(img, encodedFileDCT, blockSize, quantLevel);
    auto t_enc2_end = std::chrono::high_resolution_clock::now();
    auto t_dec2_start = std::chrono::high_resolution_clock::now();
    cv::Mat decodedDCT = decodeImageDCT(encodedFileDCT, 0, 0, 0, 0);  // ok, usa header
    auto t_dec2_end = std::chrono::high_resolution_clock::now();
    double mse2 = calculateMSE(img, decodedDCT);
    double psnr2 = calculatePSNR(mse2);
    size_t encodedSize2 = getFileSize(encodedFileDCT);
    double compressao2 = inputSize > 0 ? 100.0 * (1.0 - (double)encodedSize2 / inputSize) : 0.0;

    std::cout << "==== Métricas Delivery 4 - T4 ====" << std::endl;
    std::cout << "[Quantização Simples]" << std::endl;
    std::cout << "Tamanho original: " << inputSize << " bytes" << std::endl;
    std::cout << "Tamanho codificado: " << encodedSize1 << " bytes" << std::endl;
    std::cout << "Compressão: " << compressao1 << "%" << std::endl;
    std::cout << "MSE: " << mse1 << std::endl;
    std::cout << "PSNR: " << psnr1 << " dB" << std::endl;
    std::cout << "Tempo de codificação: " << std::chrono::duration_cast<std::chrono::milliseconds>(t_enc1_end - t_enc1_start).count() << " ms" << std::endl;
    std::cout << "Tempo de descodificação: " << std::chrono::duration_cast<std::chrono::milliseconds>(t_dec1_end - t_dec1_start).count() << " ms" << std::endl;
    std::cout << "-----------------------------" << std::endl;
    std::cout << "[DCT + ZigZag + Quantização]" << std::endl;
    std::cout << "Tamanho original: " << inputSize << " bytes" << std::endl;
    std::cout << "Tamanho codificado: " << encodedSize2 << " bytes" << std::endl;
    std::cout << "Compressão: " << compressao2 << "%" << std::endl;
    std::cout << "MSE: " << mse2 << std::endl;
    std::cout << "PSNR: " << psnr2 << " dB" << std::endl;
    std::cout << "Tempo de codificação: " << std::chrono::duration_cast<std::chrono::milliseconds>(t_enc2_end - t_enc2_start).count() << " ms" << std::endl;
    std::cout << "Tempo de descodificação: " << std::chrono::duration_cast<std::chrono::milliseconds>(t_dec2_end - t_dec2_start).count() << " ms" << std::endl;
    std::cout << "=============================" << std::endl;

    cv::imshow("Original", img);
    cv::imshow("Decodificada Quantizacao", decodedLossy);
    cv::imshow("Decodificada DCT", decodedDCT);
    cv::waitKey(0);

    return 0;
}

