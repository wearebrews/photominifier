#include <iostream>
#include <stdlib.h>
#include "curl/curl.h"
#include "photorepository.hpp"
#include <memory>
#include <fstream>
#include "optimize_jpeg.hpp"


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
                auto buffer = rep.Download(p);
                double before = buffer.size();
                std::cout << "Size before: " << before / (1024.0*1024.0) << std::endl;
                jpeg::optimize(buffer);
                std::cout << "Size after: " << buffer.size() / (1024.0*1024.0) << " (" << (1 - buffer.size()  / before)*100 << "% reduction)" << std::endl;
                std::ofstream s;
                s.open("test.jpeg", std::ios::out | std::ios::binary);
                s.write((const char*) buffer.data(), buffer.size());
            } catch(const std::exception& e) {
                std::cout << "Error: " << e.what() << std::endl;
            }
        }
    }
    return 0;
}
