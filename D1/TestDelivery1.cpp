#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>

//  g++ TestDelivery1.cpp -o TestDelivery1 `pkg-config --cflags --libs opencv4`
//  ./TestDelivery1 img1.jpeg (1 imagem)
//  ./TestDelivery1 img1.jpeg img2.jpeg   (comparar 2 imagens)

// Função para calcular o histograma de uma imagem em escala de cinzento
void printHistogram(const cv::Mat& grayImage) {
    int histSize = 256;
    float range[] = {0, 256};
    const float* histRange = {range};
    cv::Mat hist;
    cv::calcHist(&grayImage, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);
    std::cout << "\nHistograma (primeiros 16 valores): ";
    for (int i = 0; i < 16; ++i) {
        std::cout << hist.at<float>(i) << " ";
    }
    std::cout << "..." << std::endl;
}

// Função para aplicar filtro Gaussiano e mostrar métricas
void testGaussianBlur(const cv::Mat& image) {
    std::vector<int> kernelSizes = {3, 7, 15};
    std::cout << "\n--- Teste de Filtro Gaussiano ---" << std::endl;
    for (int k : kernelSizes) {
        cv::Mat blurred;
        cv::GaussianBlur(image, blurred, cv::Size(k, k), 0);
        double mse = cv::norm(image, blurred, cv::NORM_L2SQR) / (image.total());
        double psnr = (mse == 0) ? 100.0 : 10.0 * log10(255 * 255 / mse);
        std::cout << "Kernel " << k << "x" << k << " | MSE: " << mse << " | PSNR: " << psnr << " dB" << std::endl;
    }
}

// Função para quantizar imagem
void quantize(cv::Mat& image, int q) {
    int r = 256 / q;
    for (int row = 0; row < image.rows; row++)
        for (int col = 0; col < image.cols; col++)
            image.at<uchar>(row, col) = (image.at<uchar>(row, col) / r) * r;
}

// Função para calcular MSE
double calculateMSE(const cv::Mat& img1, const cv::Mat& img2) {
    cv::Mat diff;
    cv::absdiff(img1, img2, diff);
    diff.convertTo(diff, CV_32F);
    diff = diff.mul(diff);
    return cv::mean(diff)[0];
}

// Função para calcular PSNR
double calculatePSNR(double mse) {
    if (mse == 0) return 100.0;
    return 10.0 * log10((255 * 255) / mse);
}

// Função para testar quantização
void testQuantization(const cv::Mat& grayImage) {
    std::cout << "\n--- Teste de Quantização ---" << std::endl;
    for (int q = 2; q <= 16; q *= 2) {
        cv::Mat quantized = grayImage.clone();
        quantize(quantized, q);
        double mse = calculateMSE(grayImage, quantized);
        double psnr = calculatePSNR(mse);
        std::cout << "Níveis: " << q << " | MSE: " << mse << " | PSNR: " << psnr << " dB" << std::endl;
    }
}

// Função para testar diferença entre duas imagens
void testDifference(const cv::Mat& img1, const cv::Mat& img2) {
    std::cout << "\n--- Teste de Diferença entre Imagens ---" << std::endl;
    if (img1.size() != img2.size() || img1.type() != img2.type()) {
        std::cout << "Imagens incompatíveis para comparação!" << std::endl;
        return;
    }
    double mse = calculateMSE(img1, img2);
    double psnr = calculatePSNR(mse);
    std::cout << "MSE: " << mse << " | PSNR: " << psnr << " dB" << std::endl;
}

int main(int argc, char** argv) {
    using namespace std::chrono;
    std::cout << "==== Teste Delivery 1 ====" << std::endl;
    if (argc < 2) {
        std::cout << "Uso: " << argv[0] << " <imagem1> [imagem2]" << std::endl;
        return -1;
    }
    
    auto t_start = high_resolution_clock::now();
    // Carregar imagem principal
    auto t_load_start = high_resolution_clock::now();
    cv::Mat image = cv::imread(argv[1], cv::IMREAD_COLOR);
    auto t_load_end = high_resolution_clock::now();
    if (image.empty()) {
        std::cout << "Erro ao carregar a imagem principal!" << std::endl;
        return -1;
    }
    std::cout << "Imagem carregada: " << argv[1] << " (" << image.cols << "x" << image.rows << ")" << std::endl;
    std::cout << "Tempo de carregamento: " << duration_cast<milliseconds>(t_load_end - t_load_start).count() << " ms" << std::endl;

    // Separar canais e converter para cinzento
    auto t_gray_start = high_resolution_clock::now();
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
    auto t_gray_end = high_resolution_clock::now();
    std::cout << "Imagem convertida para escala de cinzento." << std::endl;
    std::cout << "Tempo de conversão para cinzento: " << duration_cast<milliseconds>(t_gray_end - t_gray_start).count() << " ms" << std::endl;

    // Histograma
    auto t_hist_start = high_resolution_clock::now();
    printHistogram(grayImage);
    auto t_hist_end = high_resolution_clock::now();
    std::cout << "Tempo de cálculo do histograma: " << duration_cast<milliseconds>(t_hist_end - t_hist_start).count() << " ms" << std::endl;

    // Filtros
    auto t_filtros_start = high_resolution_clock::now();
    testGaussianBlur(grayImage);
    auto t_filtros_end = high_resolution_clock::now();
    std::cout << "Tempo de filtros gaussianos: " << duration_cast<milliseconds>(t_filtros_end - t_filtros_start).count() << " ms" << std::endl;

    // Quantização
    auto t_quant_start = high_resolution_clock::now();
    testQuantization(grayImage);
    auto t_quant_end = high_resolution_clock::now();
    std::cout << "Tempo de quantização: " << duration_cast<milliseconds>(t_quant_end - t_quant_start).count() << " ms" << std::endl;

    // Se fornecida, comparar com segunda imagem
    if (argc >= 3) {
        auto t_cmp_start = high_resolution_clock::now();
        cv::Mat image2 = cv::imread(argv[2], cv::IMREAD_COLOR);
        if (image2.empty()) {
            std::cout << "Erro ao carregar a segunda imagem!" << std::endl;
        } else {
            cv::Mat gray2;
            cv::cvtColor(image2, gray2, cv::COLOR_BGR2GRAY);
            testDifference(grayImage, gray2);
        }
        auto t_cmp_end = high_resolution_clock::now();
        std::cout << "Tempo de comparação entre imagens: " << duration_cast<milliseconds>(t_cmp_end - t_cmp_start).count() << " ms" << std::endl;
    }
    auto t_end = high_resolution_clock::now();
    std::cout << "\nTempo total de execução: " << duration_cast<milliseconds>(t_end - t_start).count() << " ms" << std::endl;
    std::cout << "\n==== Fim dos Testes ====" << std::endl;
    return 0;
} 