#include <opencv2/opencv.hpp>
#include "../common/BitStream.h"
#include "../common/Golomb.h"
#include <iostream>

using namespace std;
using namespace cv;

int predictor(Mat& channel, int y, int x) {
    int A = (x > 0) ? channel.at<uchar>(y, x - 1) : 0;
    int B = (y > 0) ? channel.at<uchar>(y - 1, x) : 0;
    int C = (x > 0 && y > 0) ? channel.at<uchar>(y - 1, x - 1) : 0;
    return A + B - C;
}

void decodeImage(const string& inputBinPath, const string& outputImagePath) {
    BitStream bs(inputBinPath, BitStream::Read);
    int cols = bs.readBits(16);
    int rows = bs.readBits(16);
    int m = bs.readBits(8);

    vector<Mat> channels(3, Mat(rows, cols, CV_8UC1));
    Golomb golomb(m);

    for (int c = 0; c < 3; ++c) {
        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols; ++x) {
                int predicted = predictor(channels[c], y, x);
                int residual = golomb.decode(bs);
                int value = predicted + residual;
                value = std::clamp(value, 0, 255);
                channels[c].at<uchar>(y, x) = static_cast<uchar>(value);
            }
        }
    }

    Mat image;
    merge(channels, image);
    imwrite(outputImagePath, image);

    bs.close();
}