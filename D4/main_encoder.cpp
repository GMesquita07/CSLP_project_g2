#include <iostream>
#include "encoder/LosslessImageEncoder.cpp" // ou criar header se preferires

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <input_image> <output_bin> <golomb_m>" << std::endl;
        return 1;
    }

    try {
        encodeImage(argv[1], argv[2], std::stoi(argv[3]));
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
