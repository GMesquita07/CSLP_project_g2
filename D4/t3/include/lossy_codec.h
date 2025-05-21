#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

void encodeImageLossy(const cv::Mat& img, const std::string& filename, int m, int quantLevel);
cv::Mat decodeImageLossy(const std::string& filename, int rows, int cols, int m, int quantLevel);

// Funções para DCT-based coding
void encodeImageDCT(const cv::Mat& img, const std::string& filename, int blockSize, int quantLevel);
cv::Mat decodeImageDCT(const std::string& filename, int rows, int cols, int blockSize, int quantLevel);

// Zig-zag order helpers
std::vector<int> zigZagOrder(const cv::Mat& block);
cv::Mat inverseZigZagOrder(const std::vector<int>& vec, int blockSize);
