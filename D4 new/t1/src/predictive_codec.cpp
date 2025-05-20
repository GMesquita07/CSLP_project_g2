// predictive_codec.cpp
#include "predictive_codec.h"
#include "predictor.h"
#include "Golomb.h"
#include <opencv2/opencv.hpp>
#include <fstream>

void encodeImageLossless(const cv::Mat& img, const std::string& filename, int m) {
    BitStream bs(filename, true);
    GolombEncoder encoder(bs, m);

    for (int c = 0; c < 3; ++c) { // For each color channel
        for (int i = 0; i < img.rows; ++i) {
            for (int j = 0; j < img.cols; ++j) {
                int current = img.at<cv::Vec3b>(i, j)[c];
                int pred = predictor(img, i, j, c);
                int residual = current - pred;
                encoder.encodeSigned(residual);
            }
        }
    }
    bs.flush();
}

cv::Mat decodeImageLossless(const std::string& filename, int rows, int cols, int m) {
    BitStream bs(filename, false);
    GolombDecoder decoder(bs, m);
    cv::Mat img(rows, cols, CV_8UC3);

    for (int c = 0; c < 3; ++c) {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                int pred = predictor(img, i, j, c);
                int residual = decoder.decodeSigned();
                int value = pred + residual;
                img.at<cv::Vec3b>(i, j)[c] = cv::saturate_cast<uchar>(value);
            }
        }
    }
    return img;
}
