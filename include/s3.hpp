#pragma once
#include <vector>
#include <string>

namespace photorepository {
    struct Photograph {
        std::string file_name;
    };
    class Repository {
        public:
            std::vector<Photograph> list_images();
    };
}