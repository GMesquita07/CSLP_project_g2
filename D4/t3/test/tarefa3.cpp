#include "video_codec.h"   
#include <iostream>

int main() {
    std::string inputVideo = "../data/testvideo.mp4";              // para testar com video original, mudar testvideo -> testvideo15seg  (EXTREMAMENTE MAIS DEMORADO)
    std::string encodedFile = "encoded_interframe.bin";
    std::string outputVideo = "decoded_interframe.avi";

    int m = 8;              // parâmetro Golomb
    int blockSize = 8;      // tamanho do bloco para compensação de movimento
    int searchArea = 4;    // área de busca ±15 pixels
    int IFramePeriod = 10;  

    std::cout << "A codificar vídeo com codificação inter-frame...\n";
    encodeVideoInterFrame(inputVideo, encodedFile, m, blockSize, searchArea, IFramePeriod);
    std::cout << "Vídeo codificado com sucesso.\n";

    std::cout << "A descodificar vídeo...\n";
    decodeVideoInterFrame(encodedFile, outputVideo);
    std::cout << "Vídeo descodificado com sucesso.\n";

    return 0;
}
