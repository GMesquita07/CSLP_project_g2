#include "lossy_codec.h"
#include "predictor.h"
#include "BitStream.h"
#include "Golomb.h"
#include <opencv2/opencv.hpp>
#include <cmath>
#include <iostream>

void encodeImageLossy(const cv::Mat& img, const std::string& filename, int m, int quantLevel) {
    BitStream bs(filename, true);
    GolombEncoder encoder(bs, m);

    for (int c = 0; c < 3; ++c) {
        for (int i = 0; i < img.rows; ++i) {
            for (int j = 0; j < img.cols; ++j) {
                int current = img.at<cv::Vec3b>(i, j)[c];
                int pred = predictor(img, i, j, c);
                int residual = current - pred;

                // Quantização
                int qResidual = residual / quantLevel;

                encoder.encodeSigned(qResidual);
            }
        }
    }
    bs.flush();
}

cv::Mat decodeImageLossy(const std::string& filename, int rows, int cols, int m, int quantLevel) {
    BitStream bs(filename, false);
    GolombDecoder decoder(bs, m);
    cv::Mat img(rows, cols, CV_8UC3);

    for (int c = 0; c < 3; ++c) {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                int pred = predictor(img, i, j, c);
                int qResidual = decoder.decodeSigned();

                // Desquantização
                int residual = qResidual * quantLevel;

                int value = pred + residual;
                img.at<cv::Vec3b>(i, j)[c] = cv::saturate_cast<uchar>(value);
            }
        }
    }
    return img;
}

// Funções Zig-Zag para 8x8 blocos
static const int zigzagIndex[64] = {
  0, 1, 5, 6,14,15,27,28,
  2, 4, 7,13,16,26,29,42,
  3, 8,12,17,25,30,41,43,
  9,11,18,24,31,40,44,53,
 10,19,23,32,39,45,52,54,
 20,22,33,38,46,51,55,60,
 21,34,37,47,50,56,59,61,
 35,36,48,49,57,58,62,63
};

std::vector<int> zigZagOrder(const cv::Mat& block) {
    std::vector<int> result(64);
    for (int i = 0; i < 64; i++) {
        int r = zigzagIndex[i] / 8;
        int c = zigzagIndex[i] % 8;
        result[i] = static_cast<int>(block.at<float>(r, c));
    }
    return result;
}

cv::Mat inverseZigZagOrder(const std::vector<int>& vec, int blockSize) {
    cv::Mat block(blockSize, blockSize, CV_32F, cv::Scalar(0));
    for (int i = 0; i < vec.size(); i++) {
        int r = zigzagIndex[i] / blockSize;
        int c = zigzagIndex[i] % blockSize;
        block.at<float>(r, c) = static_cast<float>(vec[i]);
    }
    return block;
}

void encodeImageDCT(const cv::Mat& img, const std::string& filename, int blockSize, int quantLevel) {
    BitStream bs(filename, true);
    GolombEncoder encoder(bs, 8); // m = 8 default, pode ser parâmetro

    int rows = img.rows;
    int cols = img.cols;

    // Guarda header
    bs.writeInt(rows);
    bs.writeInt(cols);
    bs.writeInt(blockSize);
    bs.writeInt(quantLevel);

    for (int c = 0; c < 3; ++c) {
        for (int i = 0; i < rows; i += blockSize) {
            for (int j = 0; j < cols; j += blockSize) {
                cv::Mat block(blockSize, blockSize, CV_32F);

                // Preenche bloco com pixels convertidos para float
                for (int bi = 0; bi < blockSize; ++bi) {
                    for (int bj = 0; bj < blockSize; ++bj) {
                        int r = std::min(i + bi, rows - 1);
                        int col = std::min(j + bj, cols - 1);
                        block.at<float>(bi, bj) = static_cast<float>(img.at<cv::Vec3b>(r, col)[c]);
                    }
                }

                // Aplica DCT
                cv::dct(block, block);

                // Quantiza
                block /= quantLevel;

                // Zig-Zag
                auto zigzagVec = zigZagOrder(block);

                // Codifica os coeficientes zig-zag
                for (int val : zigzagVec) {
                    encoder.encodeSigned(val);
                }
            }
        }
    }
    bs.flush();
}

cv::Mat decodeImageDCT(const std::string& filename, int /*rows*/, int /*cols*/, int /*blockSize*/, int /*quantLevel*/) {
    BitStream bs(filename, false);
    GolombDecoder decoder(bs, 8);

    int rows = bs.readInt();
    int cols = bs.readInt();
    int blockSize = bs.readInt();
    int quantLevel = bs.readInt();

    cv::Mat img(rows, cols, CV_8UC3);

    for (int c = 0; c < 3; ++c) {
        for (int i = 0; i < rows; i += blockSize) {
            for (int j = 0; j < cols; j += blockSize) {
                std::vector<int> zigzagVec(64);
                for (int k = 0; k < 64; ++k) {
                    zigzagVec[k] = decoder.decodeSigned();
                }

                cv::Mat block = inverseZigZagOrder(zigzagVec, blockSize);

                // Desquantiza
                block *= quantLevel;

                // Aplica IDCT
                cv::idct(block, block);

                // Copia para a imagem
                for (int bi = 0; bi < blockSize; ++bi) {
                    for (int bj = 0; bj < blockSize; ++bj) {
                        int r = std::min(i + bi, rows - 1);
                        int col = std::min(j + bj, cols - 1);
                        float val = block.at<float>(bi, bj);
                        img.at<cv::Vec3b>(r, col)[c] = cv::saturate_cast<uchar>(val);
                    }
                }
            }
        }
    }
    return img;
}
