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

void encodeImage(const string& inputImagePath, const string& outputBinPath, int m) {
    Mat image = imread(inputImagePath, IMREAD_COLOR);
    if (image.empty()) {
        throw runtime_error("Could not open or find the image!");
    }

    BitStream bs(outputBinPath, BitStream::Write);
    bs.writeBits(image.cols, 16);
    bs.writeBits(image.rows, 16);
    bs.writeBits(m, 8);

    vector<Mat> channels(3);
    split(image, channels);

    Golomb golomb(m);

    for (int c = 0; c < 3; ++c) {
        for (int y = 0; y < image.rows; ++y) {
            for (int x = 0; x < image.cols; ++x) {
                int actual = channels[c].at<uchar>(y, x);
                int predicted = predictor(channels[c], y, x);
                int residual = actual - predicted;
                golomb.encode(residual, bs);
            }
        }
    }

    bs.close();
}
