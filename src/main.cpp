#include <iostream>
#include <stdlib.h>
#include "curl/curl.h"
#include "photorepository.hpp"
#include <memory>
#include <fstream>
#include "optimize_jpeg.hpp"
#include <thread>


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
                auto high = std::vector<unsigned char>(buffer.size());
                auto medium = std::vector<unsigned char>(buffer.size());
                auto low = std::vector<unsigned char>(buffer.size());
                auto instagram = std::vector<unsigned char>(buffer.size());

                auto optimize = [&buffer = std::as_const(buffer)](std::vector<unsigned char>& out, int quality = 75, int pixels = -1) {
                    jpeg::optimize(buffer, out, quality, pixels);
                };

                std::thread t_high(optimize, std::ref(high));
                std::thread t_medium(optimize, std::ref(medium), 75, 1920*1080);
                std::thread t_low(optimize, std::ref(low), 50, 1000*1000);
                std::thread t_instagram(optimize, std::ref(instagram), 50, 500*500);

                t_high.join();
                t_medium.join();
                t_low.join();
                t_instagram.join();
                auto filename = [old = p.FileName()](std::string quality) {
                    auto f = old;
                    f.replace(f.find("fullsize"), std::string("fullsize").size(), quality);
                    return f;
                };
                rep.Upload(high, {filename("high")});
                rep.Upload(medium, {filename("medium")});
                rep.Upload(low, {filename("low")});
                rep.Upload(instagram, {filename("instagram")});
            } catch(const std::exception& e) {
                std::cout << "Error: " << e.what() << std::endl;
            }
        }
    }
    return 0;
}
