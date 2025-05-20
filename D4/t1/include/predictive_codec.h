// predictive_codec.h
#pragma once

#include <string>
#include <opencv2/opencv.hpp>

void encodeImageLossless(const cv::Mat& img, const std::string& filename, int m);
cv::Mat decodeImageLossless(const std::string& filename, int rows, int cols, int m);
