#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char** argv) {
    // Verifica se o caminho da imagem foi fornecido
    if (argc != 2) {
        std::cout << "Uso: " << argv[0] << " <caminho_da_imagem>" << std::endl;
        return -1;
    }

    // Carregar a imagem em cores
    cv::Mat image = cv::imread(argv[1], cv::IMREAD_COLOR);
    
    if (image.empty()) {
        std::cout << "Erro ao carregar a imagem!" << std::endl;
        return -1;
    }

    // Exibir imagem original
    cv::imshow("Imagem Original", image);

    // Aplicar Gaussian Blur com diferentes tamanhos de kernel
    cv::Mat blurredImage1, blurredImage2, blurredImage3;
    cv::GaussianBlur(image, blurredImage1, cv::Size(3, 3), 0);
    cv::GaussianBlur(image, blurredImage2, cv::Size(7, 7), 0);
    cv::GaussianBlur(image, blurredImage3, cv::Size(15, 15), 0);

    // Exibir imagens suavizadas
    cv::imshow("Gaussian Blur (3x3)", blurredImage1);
    cv::imshow("Gaussian Blur (7x7)", blurredImage2);
    cv::imshow("Gaussian Blur (15x15)", blurredImage3);

    // Aguarde uma tecla para sair
    cv::waitKey(0);
    
    return 0;
}
