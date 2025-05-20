cmake_minimum_required(VERSION 3.10)
project(D4PredictiveCoding)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(
    include
    ${OpenCV_INCLUDE_DIRS}
)




# Executável para Tarefa 4 (lossy coding + DCT)
add_executable(delivery4
    src/predictive_codec.cpp
    src/predictor.cpp
    src/BitStream.cpp
    src/Golomb.cpp
    src/metrics.cpp
    src/lossy_codec.cpp           # novo ficheiro para quantização e DCT
    test/tarefa4.cpp             # teste para a tarefa 4
)

target_link_libraries(delivery4 ${OpenCV_LIBS})
