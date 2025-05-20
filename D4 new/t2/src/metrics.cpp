#include "metrics.h"
#include <cmath>

double calculateMSE(const cv::Mat& original, const cv::Mat& reconstructed) {
    if (original.rows != reconstructed.rows || original.cols != reconstructed.cols || original.type() != reconstructed.type()) {
        throw std::invalid_argument("Imagens devem ter o mesmo tamanho e tipo");
    }

    double mse = 0.0;
    int totalElements = original.rows * original.cols * original.channels();

    for (int i = 0; i < original.rows; ++i) {
        for (int j = 0; j < original.cols; ++j) {
            cv::Vec3b pix1 = original.at<cv::Vec3b>(i, j);
            cv::Vec3b pix2 = reconstructed.at<cv::Vec3b>(i, j);
            for (int c = 0; c < 3; ++c) {
                double diff = static_cast<double>(pix1[c]) - static_cast<double>(pix2[c]);
                mse += diff * diff;
            }
        }
    }
    return mse / totalElements;
}

double calculatePSNR(double mse) {
    if (mse == 0.0)
        return std::numeric_limits<double>::infinity();
    return 10.0 * std::log10((255.0 * 255.0) / mse);
}
