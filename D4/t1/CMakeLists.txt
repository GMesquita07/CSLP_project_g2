cmake_minimum_required(VERSION 3.10)
project(D4PredictiveCoding)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(
    include
    ${OpenCV_INCLUDE_DIRS}
)

add_executable(delivery1
    src/predictive_codec.cpp
    src/predictor.cpp
    src/BitStream.cpp
    src/Golomb.cpp
    src/metrics.cpp
    test/tarefa1.cpp
)

target_link_libraries(delivery1 ${OpenCV_LIBS})
