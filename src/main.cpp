#include <iostream>
#include <stdlib.h>
#include "curl/curl.h"
#include <memory>
#include <fstream>
#include "optimize_jpeg.hpp"
#include <thread>


const std::string TOKEN = []() { auto t = getenv("TOKEN");  return t ? t : "";}();
const std::string SECRET = [](){ auto s = getenv("SECRET"); return s ? s : "";}(); 


int main() {
    std::cout << "Hello world" << std::endl;
    return 0;
}
