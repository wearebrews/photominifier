#pragma once
#include <vector>

namespace jpeg {
    void optimize(std::vector<unsigned char>& in, int quality = 75);
}