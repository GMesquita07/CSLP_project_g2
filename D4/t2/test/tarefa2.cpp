#include "video_codec.h"
#include <iostream>

int main() {
    // Define o caminho do vídeo de entrada - muda para o teu vídeo real
    std::string inputVideo = "/home/mesquita/CSLP/CSLP_project_g2/D4 new/t2/data/testvideo.mp4";

    // Ficheiro onde vai ficar o vídeo codificado
    std::string encodedFile = "encoded_video.bin";

    // Ficheiro para o vídeo descodificado
    std::string outputVideo = "decoded_output.avi";

    int m = 8;  // parâmetro Golomb

    std::cout << "A codificar vídeo...\n";
    encodeVideoLossless(inputVideo, encodedFile, m);
    std::cout << "Vídeo codificado com sucesso.\n";

    std::cout << "A descodificar vídeo...\n";
    decodeVideoLossless(encodedFile, outputVideo);
    std::cout << "Vídeo descodificado com sucesso.\n";

    return 0;
}
