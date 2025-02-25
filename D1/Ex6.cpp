#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

void quantize (uchar *image, int width, int height, int q)
{
   const int r = 256 / q; // Garante que a imagem esteja no intervalo [0, 255]
   for (int row = 0; row < height; row++)
      for (int col = 0; col < width; col++)
         image[row * width + col] = (image[row * width + col] / r) * r; // Ajusta o valor quantizado
}

// Função para calcular o MSE
double calculateMSE(const cv::Mat& original, const cv::Mat& quantized) {
    double mse = 0.0;
    for (int row = 0; row < original.rows; row++) {
        for (int col = 0; col < original.cols; col++) {
            double diff = original.at<uchar>(row, col) - quantized.at<uchar>(row, col);
            mse += diff * diff;
        }
    }
    return mse / (original.rows * original.cols);
}

// Função para calcular o PSNR
double calculatePSNR(double mse) {
    if (mse == 0) return 100.0;
    return 10.0 * log10((255 * 255) / mse);
}

int main(int argc, char** argv) {
    // Verifica se o caminho da imagem foi fornecido
    if (argc != 2) {
        std::cout << "Uso: " << argv[0] << " <caminho_da_imagem>" << std::endl;
        return -1;
    }

    // Carregar a imagem em escala de cinza
    cv::Mat grayImage = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
    
    if (grayImage.empty()) {
        std::cout << "Erro ao carregar a imagem!" << std::endl;
        return -1;
    }

    // Exibir a imagem original em escala de cinza
    cv::imshow("Imagem em Escala de Cinza", grayImage);

    // Experimentar diferentes níveis de quantização
    for (int q = 2; q <= 16; q *= 2) {
        cv::Mat quantizedImage = grayImage.clone();
        quantize(quantizedImage.data, quantizedImage.cols, quantizedImage.rows, q);

        // Calcular MSE e PSNR
        double mse = calculateMSE(grayImage, quantizedImage);
        double psnr = calculatePSNR(mse);

        std::cout << "Níveis de quantização: " << q << " | MSE: " << mse << " | PSNR: " << psnr << " dB" << std::endl;

        // Exibir a imagem quantizada
        cv::imshow("Imagem Quantizada - Níveis: " + std::to_string(q), quantizedImage);
    }

    // Aguardar tecla para fechar
    cv::waitKey(0);

    return 0;
}
