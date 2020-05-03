#include <iostream>
#include <stdlib.h>
#include "curl/curl.h"
#include "s3.hpp"

const std::string TOKEN = []() { auto t = getenv("TOKEN");  return t ? t : "";}();
const std::string SECRET = [](){ auto s = getenv("SECRET"); return s ? s : "";}(); 

int main() {
    auto rep = photorepository::Repository(TOKEN, SECRET);
    auto photos = rep.list_images();
    for (auto& p : photos) {
        std::cout << p.id << std::endl;
    }
    return 0;
}