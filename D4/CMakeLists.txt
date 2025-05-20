cmake_minimum_required(VERSION 3.10)
project(D4)

set(CMAKE_CXX_STANDARD 17)

# Encontra o OpenCV
find_package(OpenCV REQUIRED)

# Diretórios de includes
include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/encoder
    ${CMAKE_SOURCE_DIR}/decoder
)

# Fontes comuns (bitstream + golomb)
set(COMMON_SOURCES
    common/BitStream.cpp
    common/Golomb.cpp
)

# Codificador de imagem sem perdas
add_executable(main_encoder
    main_encoder.cpp
    encoder/LosslessImageEncoder.cpp
    ${COMMON_SOURCES}
)

# Decodificador de imagem sem perdas
add_executable(main_decoder
    main_decoder.cpp
    decoder/LosslessImageDecoder.cpp
    ${COMMON_SOURCES}
)

# Liga com OpenCV
target_link_libraries(main_encoder ${OpenCV_LIBS})
target_link_libraries(main_decoder ${OpenCV_LIBS})

# (Opcional) Compilar o main.cpp geral se necessário
# add_executable(main_exec main.cpp)
# target_link_libraries(main_exec ${OpenCV_LIBS})
