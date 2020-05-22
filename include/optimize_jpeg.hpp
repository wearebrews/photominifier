#pragma once
#include <vector>

namespace jpeg {
    void optimize(std::vector<unsigned char>& in, int quality = 75, int pixels = -1);
    void optimize(const std::vector<unsigned char>& in, std::vector<unsigned char>& out, int quality = 75, int pixels = -1);
}