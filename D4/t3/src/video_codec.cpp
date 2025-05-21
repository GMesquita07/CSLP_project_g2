#include "video_codec.h"
#include "predictive_codec.h"
#include "predictor.h"
#include "BitStream.h"
#include "Golomb.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>

struct MotionVector {
    int dx;
    int dy;
};

// Função para calcular erro entre dois blocos
static int blockSAD(const cv::Mat& block1, const cv::Mat& block2) {
    int sad = 0;
    for (int i = 0; i < block1.rows; ++i) {
        for (int j = 0; j < block1.cols; ++j) {
            cv::Vec3b p1 = block1.at<cv::Vec3b>(i, j);
            cv::Vec3b p2 = block2.at<cv::Vec3b>(i, j);
            for (int c = 0; c < 3; ++c) {
                sad += std::abs(p1[c] - p2[c]);
            }
        }
    }
    return sad;
}

// Busca vetores de movimento numa área de pesquisa (±searchArea)
static MotionVector motionEstimation(const cv::Mat& currentFrame, const cv::Mat& refFrame,
                                     int x, int y, int blockSize, int searchArea) {
    MotionVector bestMV{0, 0};
    int minError = std::numeric_limits<int>::max();

    int rows = currentFrame.rows;
    int cols = currentFrame.cols;

    // bloco corrente
    cv::Rect currBlockRect(x, y, blockSize, blockSize);
    if (x + blockSize > cols) currBlockRect.width = cols - x;
    if (y + blockSize > rows) currBlockRect.height = rows - y;
    cv::Mat currentBlock = currentFrame(currBlockRect);

    // busca vetores na área de pesquisa
    for (int dy = -searchArea; dy <= searchArea; ++dy) {
        for (int dx = -searchArea; dx <= searchArea; ++dx) {
            int refX = x + dx;
            int refY = y + dy;

            // Verifica limites da referência
            if (refX < 0 || refY < 0 || refX + blockSize > cols || refY + blockSize > rows)
                continue;

            cv::Rect refBlockRect(refX, refY, blockSize, blockSize);
            cv::Mat refBlock = refFrame(refBlockRect);

            int error = blockSAD(currentBlock, refBlock);
            if (error < minError) {
                minError = error;
                bestMV.dx = dx;
                bestMV.dy = dy;
            }
        }
    }

    return bestMV;
}

// Codificação de um frame I (intra)
static void encodeIFrame(const cv::Mat& frame, BitStream& bs, int m) {
    // codificar cada pixel residual usando preditor
    GolombEncoder encoder(bs, m);
    for (int c = 0; c < 3; ++c) {
        for (int i = 0; i < frame.rows; ++i) {
            for (int j = 0; j < frame.cols; ++j) {
                int pred = predictor(frame, i, j, c);
                int residual = frame.at<cv::Vec3b>(i, j)[c] - pred;
                encoder.encodeSigned(residual);
            }
        }
    }
    encoder.flush();
}

// Decodificação de um frame I (intra)
static cv::Mat decodeIFrame(BitStream& bs, int rows, int cols, int m) {
    GolombDecoder decoder(bs, m);
    cv::Mat img(rows, cols, CV_8UC3, cv::Scalar(0));

    for (int c = 0; c < 3; ++c) {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                int pred = predictor(img, i, j, c);
                int residual = decoder.decodeSigned();
                int val = pred + residual;
                img.at<cv::Vec3b>(i, j)[c] = cv::saturate_cast<uchar>(val);
            }
        }
    }
    return img;
}

void encodeVideoInterFrame(const std::string& inputVideoFile, const std::string& encodedFile,
                          int m, int blockSize, int searchArea, int IFramePeriod) {
    cv::VideoCapture cap(inputVideoFile);
    if (!cap.isOpened()) {
        std::cerr << "Erro ao abrir vídeo: " << inputVideoFile << std::endl;
        return;
    }

    int width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    int fps = static_cast<int>(cap.get(cv::CAP_PROP_FPS));
    int frameCount = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));

    BitStream bs(encodedFile, true);

    // Escrever header
    bs.writeInt(width);
    bs.writeInt(height);
    bs.writeInt(fps);
    bs.writeInt(frameCount);
    bs.writeInt(m);
    bs.writeInt(blockSize);
    bs.writeInt(searchArea);
    bs.writeInt(IFramePeriod);

    cv::Mat prevFrame;

    for (int frameIdx = 0; frameIdx < frameCount; ++frameIdx) {
        cv::Mat frame;
        if (!cap.read(frame)) break;

        // I-frame a cada IFramePeriod frames
        bool isIFrame = (frameIdx % IFramePeriod == 0);

        // Escrever flag do tipo de frame
        bs.writeBit(isIFrame ? 1 : 0);

        if (isIFrame) {
            encodeIFrame(frame, bs, m);
        } else {
            // Codificação inter-frame (P-frame)

            // Primeiro: vetores de movimento para cada bloco
            std::vector<MotionVector> motionVectors;
            // Segundo: resíduos codificados (usando preditor)

            GolombEncoder encoder(bs, m);

            int rows = frame.rows;
            int cols = frame.cols;

            // Codifica vetores de movimento para cada bloco (dx, dy) como inteiros com sinal
            for (int y = 0; y < rows; y += blockSize) {
                for (int x = 0; x < cols; x += blockSize) {
                    MotionVector mv = motionEstimation(frame, prevFrame, x, y, blockSize, searchArea);
                    motionVectors.push_back(mv);

                    // Codificar mv.dx e mv.dy
                    encoder.encodeSigned(mv.dx);
                    encoder.encodeSigned(mv.dy);
                }
            }

            // Codificar resíduos do frame atual usando preditor inter-frame:
            // A predição é feita usando o bloco deslocado do frame anterior, conforme vetor de movimento

            int mvIndex = 0;
            for (int y = 0; y < rows; y += blockSize) {
                for (int x = 0; x < cols; x += blockSize) {
                    MotionVector mv = motionVectors[mvIndex++];
                    for (int bi = 0; bi < blockSize; ++bi) {
                        for (int bj = 0; bj < blockSize; ++bj) {
                            int r = std::min(y + bi, rows - 1);
                            int c_ = std::min(x + bj, cols - 1);

                            int predPixel = 0;
                            for (int ch = 0; ch < 3; ++ch) {
                                // Preditivo: pixel no prevFrame deslocado pelo mv
                                int refR = std::min(std::max(r + mv.dy, 0), rows -1);
                                int refC = std::min(std::max(c_ + mv.dx, 0), cols -1);
                                predPixel = prevFrame.at<cv::Vec3b>(refR, refC)[ch];

                                int currentPixel = frame.at<cv::Vec3b>(r, c_)[ch];
                                int residual = currentPixel - predPixel;
                                encoder.encodeSigned(residual);
                            }
                        }
                    }
                }
            }

            encoder.flush();
        }

        prevFrame = frame.clone();
    }

    bs.flush();
}

void decodeVideoInterFrame(const std::string& encodedFile, const std::string& outputVideoFile) {
    BitStream bs(encodedFile, false);

    int width = bs.readInt();
    int height = bs.readInt();
    int fps = bs.readInt();
    int frameCount = bs.readInt();
    int m = bs.readInt();
    int blockSize = bs.readInt();
    int searchArea = bs.readInt();
    int IFramePeriod = bs.readInt();

    cv::VideoWriter writer(outputVideoFile, cv::VideoWriter::fourcc('M','J','P','G'), fps, cv::Size(width, height));

    cv::Mat prevFrame;

    for (int frameIdx = 0; frameIdx < frameCount; ++frameIdx) {
        bool isIFrame = bs.readBit();

        if (isIFrame) {
            cv::Mat frame = decodeIFrame(bs, height, width, m);
            writer.write(frame);
            prevFrame = frame.clone();
        } else {
            // Decodificar P-frame
            int rows = height;
            int cols = width;

            int mvCount = ((rows + blockSize - 1) / blockSize) * ((cols + blockSize - 1) / blockSize);
            std::vector<MotionVector> motionVectors(mvCount);

            GolombDecoder decoder(bs, m);

            // Ler vetores de movimento
            for (int i = 0; i < mvCount; ++i) {
                motionVectors[i].dx = decoder.decodeSigned();
                motionVectors[i].dy = decoder.decodeSigned();
            }

            cv::Mat frame(height, width, CV_8UC3);

            // Ler resíduos e reconstruir frame
            int mvIndex = 0;
            for (int y = 0; y < rows; y += blockSize) {
                for (int x = 0; x < cols; x += blockSize) {
                    MotionVector mv = motionVectors[mvIndex++];
                    for (int bi = 0; bi < blockSize; ++bi) {
                        for (int bj = 0; bj < blockSize; ++bj) {
                            int r = std::min(y + bi, rows - 1);
                            int c_ = std::min(x + bj, cols - 1);

                            for (int ch = 0; ch < 3; ++ch) {
                                int refR = std::min(std::max(r + mv.dy, 0), rows -1);
                                int refC = std::min(std::max(c_ + mv.dx, 0), cols -1);

                                int predPixel = prevFrame.at<cv::Vec3b>(refR, refC)[ch];
                                int residual = decoder.decodeSigned();
                                int val = predPixel + residual;
                                frame.at<cv::Vec3b>(r, c_)[ch] = cv::saturate_cast<uchar>(val);
                            }
                        }
                    }
                }
            }

            writer.write(frame);
            prevFrame = frame.clone();
        }
    }

    writer.release();
    bs.flush();
}
