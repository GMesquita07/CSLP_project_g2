// predictor.h
#pragma once

#include <opencv2/opencv.hpp>

// JPEG-style predictor based on neighbors A (left), B (top), and C (top-left)
int jpegPredictor(int a, int b, int c);

// Wrapper to handle border conditions and get predictor from the image
int predictor(const cv::Mat& img, int i, int j, int channel);
