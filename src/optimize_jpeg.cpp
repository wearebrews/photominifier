#include <vector>
#include <array>
#include <memory>
#include "optimize_jpeg.hpp"

extern "C" {
#include "turbojpeg.h"
}

constexpr std::array<const char*, TJ_NUMSAMP> sub_samp_name = {
  "4:4:4", "4:2:2", "4:2:0", "Grayscale", "4:4:0", "4:1:1"
};

constexpr std::array<const char *, TJ_NUMCS> color_space_name = {
  "RGB", "YCbCr", "GRAY", "CMYK", "YCCK"
};

void jpeg::optimize(std::vector<unsigned char>& in, int quality) {
    int in_sub_samp, in_color_space;

    unsigned long jpeg_size;

    int n_scaling;
    auto* scaling_factors = tjGetScalingFactors(&n_scaling);
    if (!scaling_factors) {
        throw std::runtime_error("Could not get scaling factors");
    }
    auto tj_instance = tjInitDecompress();
    if (!tj_instance) {
        throw std::runtime_error("Error initiating decompress");
    }
    int height, width;
    if (tjDecompressHeader3(tj_instance, in.data(), in.size(), &width, &height, &in_sub_samp, &in_color_space) < 0) {
        throw std::runtime_error("Error retrieving metadata");
    }
    int pixel_format = TJPF_BGRX;
    auto img_buf = std::unique_ptr<unsigned char>(tjAlloc(width * height * tjPixelSize[pixel_format]));
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
    auto ptr = in.data();
    if (tjCompress2(tj_instance, img_buf.get(), width, 0, height, pixel_format, &ptr, &jpeg_size, in_sub_samp, quality, TJFLAG_NOREALLOC) < 0) {
        throw std::runtime_error("Error recompressing");
    }
    in.resize(jpeg_size);
}