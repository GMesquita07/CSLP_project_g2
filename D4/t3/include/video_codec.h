#pragma once

#include <string>

void encodeVideoInterFrame(const std::string& inputVideoFile, const std::string& encodedFile,
                          int m, int blockSize, int searchArea, int IFramePeriod);

void decodeVideoInterFrame(const std::string& encodedFile, const std::string& outputVideoFile);

