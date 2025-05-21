#pragma once

#include <opencv2/opencv.hpp>

// Calcula o Erro Quadrático Médio (Mean Squared Error)
double calculateMSE(const cv::Mat& original, const cv::Mat& reconstructed);

// Calcula o Peak Signal-to-Noise Ratio
double calculatePSNR(double mse);
