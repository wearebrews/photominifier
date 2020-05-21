#pragma once
#include <vector>
#include <string>
#include "aws/s3/S3Client.h"
#include <memory>
#include <tuple>

namespace photorepository {
    class Repository;
    class Photograph {
        private:
            std::string file_name_;
            Repository* repository_ = nullptr;
        public:
            Photograph(std::string file_name = "") : file_name_(file_name) {}
            const std::string FileName() const { return file_name_;}
    };

    class Repository {
        private:
            Aws::S3::S3Client s3_client_;
        public:
            Repository(std::string token, std::string secret, std::string endpoint = "https://fra1.digitaloceanspaces.com");
            ~Repository();
            std::vector<Photograph> List(std::string = "");
            const std::vector<char> Download(const Photograph& photo);
    };
}