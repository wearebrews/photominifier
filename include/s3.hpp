#pragma once
#include <vector>
#include <string>
#include "aws/s3/S3Client.h"

namespace photorepository {
    struct Photograph {
        std::string id;
        std::string file_name;
    };
    class Repository {
        private:
            Aws::S3::S3Client s3_client_;
        public:
            Repository(std::string token, std::string secret, std::string endpoint);
            Repository(std::string token, std::string secret);
            ~Repository();
            std::vector<Photograph> list_images();
    };
}