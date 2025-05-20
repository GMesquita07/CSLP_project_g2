#pragma once

#include <string>

void encodeVideoLossless(const std::string& inputVideoFile, const std::string& encodedFile, int m);
void decodeVideoLossless(const std::string& encodedFile, const std::string& outputVideoFile);
