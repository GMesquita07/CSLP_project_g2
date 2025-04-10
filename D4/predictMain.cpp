#include <iostream>
#include <opencv2/opencv.hpp>

// Declarações das funções que estão em outros ficheiros:
cv::Mat decodeImage(const std::string &inFile);
void encodeImage(const cv::Mat &image, const std::string &outFile);
void calcMSE_PSNR(const cv::Mat &img1, const cv::Mat &img2, double &mse, double &psnr);

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cout << "Uso: " << argv[0] 
                  << " <imagem_entrada> <arquivo_bin> <imagem_reconstruida>\n";
        return -1;
    }

    std::string inImage     = argv[1];
    std::string outBinFile  = argv[2];
    std::string outRecons   = argv[3];

    // Ler imagem
    cv::Mat original = cv::imread(inImage, cv::IMREAD_COLOR);
    if (original.empty()) {
        std::cerr << "Erro ao abrir " << inImage << std::endl;
        return -1;
    }
    std::cout << "Imagem original: " << original.cols 
              << "x" << original.rows 
              << ", canais=" << original.channels() << "\n";

    // Codificar
    try {
        encodeImage(original, outBinFile);
        std::cout << "[main] encodeImage OK\n";
    } catch (std::exception &e) {
        std::cerr << "Erro em encodeImage: " << e.what() << std::endl;
        return -1;
    }

    // Decodificar
    cv::Mat reconstruida;
    try {
        reconstruida = decodeImage(outBinFile);
        std::cout << "[main] decodeImage OK\n";
    } catch (std::exception &e) {
        std::cerr << "Erro em decodeImage: " << e.what() << std::endl;
        return -1;
    }

    // Salvar resultado
    cv::imwrite(outRecons, reconstruida);
    std::cout << "Imagem reconstruída salva em: " << outRecons << "\n";

    // Comparar com MSE e PSNR
    double mse=0.0, psnr=0.0;
    calcMSE_PSNR(original, reconstruida, mse, psnr);
    std::cout << "MSE  = " << mse  << "\n";
    std::cout << "PSNR = " << psnr << " dB\n";

    return 0;
}
