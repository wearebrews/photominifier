#include "s3.hpp"

using namespace photorepository;

std::vector<Photograph> Repository::list_images() {
    std::vector<Photograph> vec;
    vec.push_back({.file_name="Hello"});
    return vec;
}