#include "video_codec.h"
#include "predictive_codec.h"

#include <opencv2/opencv.hpp>
#include <fstream>
#include <iostream>
#include <cstdio>  // for std::remove

void encodeVideoLossless(const std::string& inputVideoFile, const std::string& encodedFile, int m) {
    cv::VideoCapture cap(inputVideoFile);
    if (!cap.isOpened()) {
        std::cerr << "Erro ao abrir vídeo: " << inputVideoFile << std::endl;
        return;
    }

    int width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    int fps = static_cast<int>(cap.get(cv::CAP_PROP_FPS));
    int frameCount = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));

    std::ofstream ofs(encodedFile, std::ios::binary);
    if (!ofs) {
        std::cerr << "Erro ao abrir ficheiro para escrita: " << encodedFile << std::endl;
        return;
    }

    // Guarda header: width, height, fps, frameCount, m
    ofs.write(reinterpret_cast<const char*>(&width), sizeof(width));
    ofs.write(reinterpret_cast<const char*>(&height), sizeof(height));
    ofs.write(reinterpret_cast<const char*>(&fps), sizeof(fps));
    ofs.write(reinterpret_cast<const char*>(&frameCount), sizeof(frameCount));
    ofs.write(reinterpret_cast<const char*>(&m), sizeof(m));
    ofs.close();

    int frameIndex = 0;
    while (true) {
        cv::Mat frame;
        bool ret = cap.read(frame);
        if (!ret || frame.empty()) break;

        std::string tempEncoded = "frame_" + std::to_string(frameIndex) + ".bin";
        encodeImageLossless(frame, tempEncoded, m);

        std::ifstream ifs(tempEncoded, std::ios::binary);
        std::vector<char> buffer((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        ifs.close();

        std::ofstream ofsAppend(encodedFile, std::ios::binary | std::ios::app);
        int frameSize = static_cast<int>(buffer.size());
        ofsAppend.write(reinterpret_cast<const char*>(&frameSize), sizeof(frameSize));
        ofsAppend.write(buffer.data(), frameSize);
        ofsAppend.close();

        std::remove(tempEncoded.c_str());
        frameIndex++;
    }
}

void decodeVideoLossless(const std::string& encodedFile, const std::string& outputVideoFile) {
    std::ifstream ifs(encodedFile, std::ios::binary);
    if (!ifs) {
        std::cerr << "Erro ao abrir ficheiro codificado para leitura: " << encodedFile << std::endl;
        return;
    }

    int width, height, fps, frameCount, m;
    ifs.read(reinterpret_cast<char*>(&width), sizeof(width));
    ifs.read(reinterpret_cast<char*>(&height), sizeof(height));
    ifs.read(reinterpret_cast<char*>(&fps), sizeof(fps));
    ifs.read(reinterpret_cast<char*>(&frameCount), sizeof(frameCount));
    ifs.read(reinterpret_cast<char*>(&m), sizeof(m));

    cv::VideoWriter writer(outputVideoFile, cv::VideoWriter::fourcc('M','J','P','G'), fps, cv::Size(width, height));

    for (int i = 0; i < frameCount; ++i) {
        int frameSize;
        if (!ifs.read(reinterpret_cast<char*>(&frameSize), sizeof(frameSize))) {
            std::cerr << "Erro: falha a ler o tamanho do frame #" << i << std::endl;
            break;
        }

        std::vector<char> buffer(frameSize);
        if (!ifs.read(buffer.data(), frameSize)) {
            std::cerr << "Erro: falha a ler dados do frame #" << i << std::endl;
            break;
        }
        
        std::string tempEncoded = "temp_frame_" + std::to_string(i) + ".bin";
        std::ofstream ofsTemp(tempEncoded, std::ios::binary);
        ofsTemp.write(buffer.data(), frameSize);
        ofsTemp.close();

        cv::Mat decodedFrame = decodeImageLossless(tempEncoded, height, width, m);
        writer.write(decodedFrame);

        std::remove(tempEncoded.c_str());
    }

    writer.release();
    ifs.close();
}
