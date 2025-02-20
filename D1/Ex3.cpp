#include <opencv2/opencv.hpp>
#include <iostream>

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

    // Calcular o histograma
    int histSize = 256;  // Número de bins (valores de intensidade de 0 a 255)
    float range[] = {0, 256}; 
    const float* histRange = {range};

    cv::Mat hist;
    cv::calcHist(&grayImage, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);

    // Normalizar o histograma para caber na tela
    int histW = 512, histH = 400;
    int binW = cvRound((double) histW / histSize);

    cv::Mat histImage(histH, histW, CV_8UC3, cv::Scalar(0, 0, 0));
    cv::normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX);

    // Desenhar o histograma
    for (int i = 1; i < histSize; i++) {
        cv::line(histImage, 
                 cv::Point(binW * (i - 1), histH - cvRound(hist.at<float>(i - 1))),
                 cv::Point(binW * i, histH - cvRound(hist.at<float>(i))),
                 cv::Scalar(255, 255, 255), 2, 8, 0);
    }

    // Exibir o histograma
    cv::imshow("Histograma da Imagem", histImage);

    // Aguardar a tecla para fechar as janelas
    cv::waitKey(0);
    
    return 0;
}

//compilar:     g++ DisplayHistogram.cpp -o DisplayHistogram `pkg-config --cflags --libs opencv4`
                
