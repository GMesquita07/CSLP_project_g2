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

    // Separar os canais BGR
    cv::Mat channels[3];
    cv::split(image, channels);

    // Criar imagens para visualizar os canais separadamente
    cv::Mat blueChannel, greenChannel, redChannel;
    
    // Preenchendo os canais individuais para visualizar separadamente
    cv::Mat zeros = cv::Mat::zeros(image.size(), CV_8UC1);

    // Criando imagens com apenas um canal de cor
    std::vector<cv::Mat> blue = {channels[0], zeros, zeros};
    std::vector<cv::Mat> green = {zeros, channels[1], zeros};
    std::vector<cv::Mat> red = {zeros, zeros, channels[2]};

    // Mesclando para criar imagens coloridas dos canais
    cv::merge(blue, blueChannel);
    cv::merge(green, greenChannel);
    cv::merge(red, redChannel);

    // Converter para escala de cinza
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

    // Exibir canais individuais e a imagem em escala de cinza
    cv::imshow("Canal Azul", blueChannel);
    cv::imshow("Canal Verde", greenChannel);
    cv::imshow("Canal Vermelho", redChannel);
    cv::imshow("Imagem em Escala de Cinza", grayImage);

    // Aguarde uma tecla para sair
    cv::waitKey(0);
    
    return 0;
}


//compilar:     g++ DisplayImage.cpp -o DisplayImage `pkg-config --cflags --libs opencv4`
//correr:       ./DisplayImage caminho_da_sua_imagem.jpg
