#include <opencv2/opencv.hpp>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <vector>

// Ajuste o caminho dos includes conforme sua organização
// Se Golomb.h e BitStream.h estão na pasta D2, por exemplo:
#include "../D2/BitStream.h"
#include "../D3/Golomb.h"

// Predictor (exemplo de JPEG-LS)
int predictor(int left, int top, int topLeft) {
    if (topLeft >= std::max(left, top))
        return std::min(left, top);
    else if (topLeft <= std::min(left, top))
        return std::max(left, top);
    else
        return left + top - topLeft;
}

/*
 * Estimate um "m" simples, baseado no valor médio dos resíduos.
 * Se já tiver uma lógica diferente, pode usar a sua.
 */
int estimateOptimalM(const std::vector<int>& residuals) {
    if (residuals.empty()) return 1;
    double soma = 0.0;
    for (auto r : residuals) {
        soma += std::abs(r);
    }
    double media = soma / residuals.size();
    int m = (int)std::round(media);
    if (m < 1) m = 1;
    return m;
}

/*
 * Codificação de imagem:
 *  1. Percorre a imagem pixel a pixel, calcula residual = real - predição.
 *  2. Usa Golomb::encode para cada residual e escreve num BitStream.
 *  3. Antes de tudo, escreve no BitStream: width, height, m, e possivelmente info de quantos canais.
 */
void encodeImage(const cv::Mat &image, const std::string &outFile) {
    if (image.empty()) {
        throw std::runtime_error("encodeImage: imagem vazia");
    }

    // Cria um BitStream em modo escrita
    BitStream bs(outFile, true /*writeMode*/);

    // Para facilitar, vamos supor grayscale. Se for colorido, basta repetir para cada canal.
    // Se tiver multi-canal, guarde também o número de canais no bitstream.
    int rows = image.rows;
    int cols = image.cols;
    int channels = image.channels();

    // Escreve alguns metadados no arquivo:
    //   16 bits: largura
    //   16 bits: altura
    //   8 bits : channels
    if (cols >= (1 << 16) || rows >= (1 << 16)) {
        throw std::runtime_error("encodeImage: dimensões muito grandes para 16 bits neste exemplo.");
    }
    bs.writeBits(cols, 16);
    bs.writeBits(rows, 16);
    bs.writeBits(channels, 8);

    // Junta todos os resíduos num vetor para poder estimar "m"
    std::vector<int> allResiduals;
    allResiduals.reserve(rows * cols * channels);

    // Percorrer a imagem
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            for (int c = 0; c < channels; c++) {
                // Pega valor "left", "top", "topLeft"
                int left = 0, top = 0, topLeft = 0;

                if (x > 0)
                    left = image.ptr<uchar>(y)[(x-1)*channels + c];
                if (y > 0)
                    top = image.ptr<uchar>(y-1)[x*channels + c];
                if (x > 0 && y > 0)
                    topLeft = image.ptr<uchar>(y-1)[(x-1)*channels + c];

                int pred = predictor(left, top, topLeft);
                int realVal = image.ptr<uchar>(y)[x*channels + c];
                int residual = realVal - pred;

                allResiduals.push_back(residual);
            }
        }
    }

    // Estima m
    int m = estimateOptimalM(allResiduals);
    // Escreve "m" (16 bits só pra exemplo; dependendo do seu range, pode ser menor)
    bs.writeBits(m, 16);

    // Cria um objeto Golomb
    Golomb gol(m);

    // Agora, percorre de novo e codifica cada residual no BitStream
    size_t idxResid = 0;
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            for (int c = 0; c < channels; c++) {
                int r = allResiduals[idxResid++];
                gol.encode(r, bs);
            }
        }
    }

    // Alinha ao byte e fecha
    bs.alignToByte();
    bs.close();

    std::cout << "[encodeImage] Concluído. Arquivo gerado: " << outFile << std::endl;
}

/*
 * Decodificação:
 *  1. Lê metadados (largura, altura, canais, m).
 *  2. Para cada pixel, lê residual, soma com a predição, e reconstrói o valor real.
 */
cv::Mat decodeImage(const std::string &inFile) {
    // Abre BitStream em modo leitura
    BitStream bs(inFile, false);

    // Ler metadados
    int cols = (int)bs.readBits(16);
    int rows = (int)bs.readBits(16);
    int channels = (int)bs.readBits(8);

    int m = (int)bs.readBits(16);

    // Criar obj Golomb
    Golomb gol(m);

    // Mat de saída
    cv::Mat outImg(rows, cols, (channels == 1 ? CV_8UC1 : CV_8UC3), cv::Scalar::all(0));

    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            for (int c = 0; c < channels; c++) {
                // Decodifica residual
                int64_t residual = gol.decode(bs);

                // Calcula preditor
                int left = 0, top = 0, topLeft = 0;
                if (x > 0)
                    left = outImg.ptr<uchar>(y)[(x-1)*channels + c];
                if (y > 0)
                    top = outImg.ptr<uchar>(y-1)[x*channels + c];
                if (x > 0 && y > 0)
                    topLeft = outImg.ptr<uchar>(y-1)[(x-1)*channels + c];

                int pred = predictor(left, top, topLeft);

                // Valor real = pred + residual
                int realVal = pred + (int)residual;
                // Clampa
                if (realVal < 0) realVal = 0;
                if (realVal > 255) realVal = 255;

                outImg.ptr<uchar>(y)[x*channels + c] = (uchar)realVal;
            }
        }
    }

    bs.close();
    return outImg;
}
