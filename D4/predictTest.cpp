#include <iostream>
#include <opencv2/opencv.hpp>

// As funções de encode/decode que criamos em predict.cpp
// Se tiver um "predict.h", inclua-o. Senão, faça extern:
cv::Mat decodeImage(const std::string &inFile);
void encodeImage(const cv::Mat &image, const std::string &outFile);

// Função para MSE e PSNR
void calcMSE_PSNR(const cv::Mat &img1, const cv::Mat &img2, double &mse, double &psnr) {
    if (img1.empty() || img2.empty() ||
        img1.size() != img2.size() || img1.type() != img2.type()) {
        mse  = 0.0;
        psnr = 0.0;
        std::cerr << "calcMSE_PSNR: imagens incompatíveis ou vazias.\n";
        return;
    }

    double sumSq = 0.0;
    int rows = img1.rows;
    int cols = img1.cols;
    int channels = img1.channels();

    for(int y = 0; y < rows; y++){
        const uchar* p1 = img1.ptr<uchar>(y);
        const uchar* p2 = img2.ptr<uchar>(y);
        for(int x = 0; x < cols*channels; x++){
            int diff = (int)p1[x] - (int)p2[x];
            sumSq += diff * diff;
        }
    }
    int total = rows * cols * channels;
    mse = sumSq / total;

    if (mse <= 0.0000001) {
        // Imagens idênticas => PSNR infinito (prático: um valor bem grande)
        psnr = 99.0;
    } else {
        psnr = 10.0 * std::log10((255.0 * 255.0) / mse);
    }
}