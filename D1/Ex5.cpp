#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

// Função para calcular o Erro Médio Quadrático (MSE)
double calculateMSE(const cv::Mat& img1, const cv::Mat& img2) {
    cv::Mat diff;
    cv::absdiff(img1, img2, diff); // Calcula a diferença absoluta entre as imagens
    diff.convertTo(diff, CV_32F);  // Converte para float para precisão nos cálculos
    diff = diff.mul(diff);         // Eleva ao quadrado os valores dos pixels

    cv::Scalar s = cv::mean(diff); // Calcula a média dos valores
    return (s[0] + s[1] + s[2]) / 3; // Média dos canais RGB
}

// Função para calcular o PSNR
double calculatePSNR(const cv::Mat& img1, const cv::Mat& img2) {
    double mse = calculateMSE(img1, img2);
    if (mse == 0) return 100; // Imagens idênticas, PSNR máximo

    double maxPixelValue = 255.0;
    return 10.0 * std::log10((maxPixelValue * maxPixelValue) / mse);
}

int main(int argc, char** argv) {
    // Verifica se os caminhos das imagens foram fornecidos
    if (argc != 3) {
        std::cout << "Uso: " << argv[0] << " <imagem1> <imagem2>" << std::endl;
        return -1;
    }

    // Carregar as duas imagens em cores
    cv::Mat image1 = cv::imread(argv[1], cv::IMREAD_COLOR);
    cv::Mat image2 = cv::imread(argv[2], cv::IMREAD_COLOR);

    // Verificar se as imagens foram carregadas corretamente
    if (image1.empty() || image2.empty()) {
        std::cout << "Erro ao carregar as imagens!" << std::endl;
        return -1;
    }

    // Verificar se as imagens têm o mesmo tamanho e tipo
    if (image1.size() != image2.size() || image1.type() != image2.type()) {
        std::cout << "Erro: As imagens devem ter o mesmo tamanho e tipo!" << std::endl;
        return -1;
    }

    // Calcular a diferença absoluta entre as imagens
    cv::Mat diff;
    cv::absdiff(image1, image2, diff);

    // Converter a diferença para escala de cinza para melhor visualização
    cv::Mat diffGray;
    cv::cvtColor(diff, diffGray, cv::COLOR_BGR2GRAY);

    // Calcular MSE e PSNR
    double mse = calculateMSE(image1, image2);
    double psnr = calculatePSNR(image1, image2);

    // Exibir resultados
    std::cout << "Erro Médio Quadrático (MSE): " << mse << std::endl;
    std::cout << "Pico de Relação Sinal-Ruído (PSNR): " << psnr << " dB" << std::endl;

    // Exibir as imagens e a diferença
    cv::imshow("Imagem 1", image1);
    cv::imshow("Imagem 2", image2);
    cv::imshow("Diferença Absoluta", diff);
    cv::imshow("Diferença em Escala de Cinza", diffGray);

    // Aguardar uma tecla para sair
    cv::waitKey(0);
    
    return 0;
}
