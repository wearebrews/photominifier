#include <iostream>
#include <stdlib.h>
#include "curl/curl.h"
#include "s3.hpp"

const std::string TOKEN = []() { auto t = getenv("TOKEN");  return t ? t : "";}();
const std::string SECRET = [](){ auto s = getenv("SECRET"); return s ? s : "";}(); 

int main() {
    auto rep = photorepository::Repository(TOKEN, SECRET);
    auto photos = rep.List("img/fullsize");
    for (auto& p : photos) {
        auto name = p.FileName();
        std::cout << p.FileName() << std::endl;
        if (name[name.length()-1] != '/') {
            try {
            rep.Download(p);
            } catch(const std::exception& e) {
                std::cout << e.what() << std::endl;
            }
        }
    }
    return 0;
}