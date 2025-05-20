#include "lossy_codec.h"
#include "metrics.h"
#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    std::string inputImage = "/home/mesquita/CSLP/CSLP_project_g2/D4/t1/data/lebron.webp";
    std::string encodedFile = "encoded_lossy.bin";
    std::string encodedFileDCT = "encoded_dct.bin";

    int m = 8;
    int quantLevel = 10;   // nível de quantização para testes
    int blockSize = 8;

    cv::Mat img = cv::imread(inputImage, cv::IMREAD_COLOR);
    if (img.empty()) {
        std::cerr << "Erro ao carregar imagem." << std::endl;
        return -1;
    }

    // Testa quantização simples dos resíduos
    encodeImageLossy(img, encodedFile, m, quantLevel);
    cv::Mat decodedLossy = decodeImageLossy(encodedFile, img.rows, img.cols, m, quantLevel);
    double mse1 = calculateMSE(img, decodedLossy);
    double psnr1 = calculatePSNR(mse1);
    std::cout << "[Quantização simples] MSE: " << mse1 << " PSNR: " << psnr1 << " dB\n";

    // Testa codificação DCT
    encodeImageDCT(img, encodedFileDCT, blockSize, quantLevel);
    cv::Mat decodedDCT = decodeImageDCT(encodedFileDCT, img.rows, img.cols, blockSize, quantLevel);
    double mse2 = calculateMSE(img, decodedDCT);
    double psnr2 = calculatePSNR(mse2);
    std::cout << "[DCT] MSE: " << mse2 << " PSNR: " << psnr2 << " dB\n";

    cv::imshow("Original", img);
    cv::imshow("Decodificada Quantizacao", decodedLossy);
    cv::imshow("Decodificada DCT", decodedDCT);
    cv::waitKey(0);

    return 0;
}
