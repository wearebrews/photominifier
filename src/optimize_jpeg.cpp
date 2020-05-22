#include <vector>
#include <array>
#include <memory>
#include "optimize_jpeg.hpp"
#include <algorithm>
#include <cmath>

#include <iostream>

extern "C" {
#include "turbojpeg.h"
}

constexpr std::array<const char*, TJ_NUMSAMP> sub_samp_name = {
  "4:4:4", "4:2:2", "4:2:0", "Grayscale", "4:4:0", "4:1:1"
};

constexpr std::array<const char *, TJ_NUMCS> color_space_name = {
  "RGB", "YCbCr", "GRAY", "CMYK", "YCCK"
};

const std::vector<tjscalingfactor> getScalingFactor() {
    int n_scaling;
    auto scaling_factors = tjGetScalingFactors(&n_scaling);
    if (!scaling_factors) {
        throw std::runtime_error("Could not get scaling factors");
    }
    
    return std::vector<tjscalingfactor>(scaling_factors, scaling_factors + n_scaling);
}

const tjscalingfactor getClosestScaling(long pixels_in, long pixels_out) {
    auto scaling_factors = getScalingFactor();
    auto target = std::sqrt(static_cast<double>(pixels_out) / static_cast<double>(pixels_in));
    auto best_scaling = scaling_factors.at(0);
    auto best_scaling_loss = INFINITY;
    for (auto& s : scaling_factors) {
        auto loss = static_cast<double>(s.num) / static_cast<double>(s.denom) - target;
        if (loss >= 0 && loss < best_scaling_loss) {
            best_scaling_loss = loss;
            best_scaling = s;
        }
    }
    if (best_scaling_loss == INFINITY) {
        throw std::runtime_error("No best scaling found");
    }
    return best_scaling;
}

void jpeg::optimize(std::vector<unsigned char>& in, int quality, int pixels) {
    jpeg::optimize(in, in, quality, pixels);
}

void jpeg::optimize(const std::vector<unsigned char>& in, std::vector<unsigned char>& out, int quality, int pixels) {
    int in_sub_samp, in_color_space;

    unsigned long jpeg_size;

   
    auto tj_instance = tjInitDecompress();
    if (!tj_instance) {
        throw std::runtime_error("Error initiating decompress");
    }
    int height, width;
    if (tjDecompressHeader3(tj_instance, in.data(), in.size(), &width, &height, &in_sub_samp, &in_color_space) < 0) {
        throw std::runtime_error("Error retrieving metadata");
    }

    if (pixels > 0) {
        auto scaling = getClosestScaling(width * height, pixels);
        height = TJSCALED(height, scaling);
        width = TJSCALED(width, scaling);
    }

    int pixel_format = TJPF_BGRX;
    auto img_buf = std::unique_ptr<unsigned char[]>(tjAlloc(width * height * tjPixelSize[pixel_format]));
    if (!img_buf) {
        throw std::runtime_error("Unable to allocate memory");
    }
    if (tjDecompress2(tj_instance, in.data(), in.size(), img_buf.get(), width, 0, height, pixel_format, 0) < 0) {
        throw std::runtime_error("Unable to decompress");
    }
    tjDestroy(tj_instance);

    tj_instance = tjInitCompress();
    if (!tj_instance) {
        throw std::runtime_error("Error initiating compressor");
    }
    auto ptr = out.data();
    if (tjCompress2(tj_instance, img_buf.get(), width, 0, height, pixel_format, &ptr, &jpeg_size, in_sub_samp, quality, TJFLAG_NOREALLOC) < 0) {
        throw std::runtime_error("Error recompressing");
    }
    out.resize(jpeg_size);
    tjDestroy(tj_instance);
}