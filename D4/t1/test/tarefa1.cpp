#include "predictive_codec.h"
#include "predictor.h"
#include "metrics.h"
#include <opencv2/opencv.hpp>

int main() {
    std::string inputImage = "/home/mesquita/CSLP/CSLP_project_g2/D4 new/t1/data/lebron.webp";;
    std::string encodedFile = "encoded_lossless.bin";

    cv::Mat img = cv::imread(inputImage, cv::IMREAD_COLOR);
    if (img.empty()) {
        std::cerr << "Erro ao carregar imagem." << std::endl;
        return -1;
    }

    int m = 8; // parâmetro Golomb
    encodeImageLossless(img, encodedFile, m);
    std::cout << "Imagem codificada com sucesso.\n";

    cv::Mat decoded = decodeImageLossless(encodedFile, img.rows, img.cols, m);
    std::cout << "Imagem descodificada com sucesso.\n";

    double mse = calculateMSE(img, decoded);
    double psnr = calculatePSNR(mse);
    std::cout << "MSE: " << mse << ", PSNR: " << psnr << " dB" << std::endl;

    cv::imshow("Original", img);
    cv::imshow("Reconstruída", decoded);
    cv::waitKey(0);
    return 0;
}