cmake_minimum_required(VERSION 3.10)
project(D4_T1_LosslessImage)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenCV REQUIRED)

include_directories(
    common
    encoder
    decoder
    ${OpenCV_INCLUDE_DIRS}
)

add_executable(lossless_image_codec
    main.cpp
    encoder/LosslessImageEncoder.cpp
    decoder/LosslessImageDecoder.cpp
    common/BitStream.cpp
    common/Golomb.cpp
)

target_link_libraries(lossless_image_codec ${OpenCV_LIBS})
