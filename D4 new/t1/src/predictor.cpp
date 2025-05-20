#include "predictor.h"
#include <algorithm> // para std::min e std::max

// JPEG-style predictor (mediana de A, B, C)
int jpegPredictor(int a, int b, int c) {
    int max_ab = std::max(a, b);
    int min_ab = std::min(a, b);
    if (c >= max_ab) return min_ab;
    if (c <= min_ab) return max_ab;
    return a + b - c;
}

// Aplica predição a um pixel específico, com controlo de borda
int predictor(const cv::Mat& img, int i, int j, int channel) {
    int a = (j > 0) ? img.at<cv::Vec3b>(i, j - 1)[channel] : 0;
    int b = (i > 0) ? img.at<cv::Vec3b>(i - 1, j)[channel] : 0;
    int c = (i > 0 && j > 0) ? img.at<cv::Vec3b>(i - 1, j - 1)[channel] : 0;
    return jpegPredictor(a, b, c);
}
