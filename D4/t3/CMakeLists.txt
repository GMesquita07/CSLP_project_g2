cmake_minimum_required(VERSION 3.10)
project(D4InterFrameCoding)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(
    include
    ${OpenCV_INCLUDE_DIRS}
)

add_executable(delivery3
    src/video_codec.cpp           # inclui as funções encodeVideoInterFrame, decodeVideoInterFrame
    src/predictive_codec.cpp
    src/predictor.cpp
    src/BitStream.cpp
    src/Golomb.cpp
    src/metrics.cpp
    test/tarefa3.cpp              # teste para tarefa 3 (inter-frame)
)

target_link_libraries(delivery3 ${OpenCV_LIBS})
