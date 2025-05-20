#include <iostream>
#include "decoder/LosslessImageDecoder.cpp"

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_bin> <output_image>" << std::endl;
        return 1;
    }

    try {
        decodeImage(argv[1], argv[2]);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
