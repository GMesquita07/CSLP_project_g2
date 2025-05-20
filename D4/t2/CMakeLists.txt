cmake_minimum_required(VERSION 3.10)
project(D4PredictiveCoding)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(
    include
    ${OpenCV_INCLUDE_DIRS}
)

add_executable(delivery2
    src/video_codec.cpp
    src/predictive_codec.cpp      # para as funções de codificação das frames
    src/predictor.cpp             # preditor usado no decode/encode
    src/BitStream.cpp
    src/Golomb.cpp
    src/metrics.cpp
    test/tarefa2.cpp              # o teste para tarefa2 (vídeo)
)

target_link_libraries(delivery2 ${OpenCV_LIBS})
