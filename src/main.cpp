#include <iostream>
#include <stdlib.h>
#include "curl/curl.h"
#include <memory>
#include "google/cloud/storage/client.h"
#include <fstream>
#include "optimize_jpeg.hpp"
#include <thread>
#include <array>


namespace gcs = google::cloud::storage;

constexpr const char* originals_prefix = "images/fullsize/";
constexpr const char* low_prefix = "images/low/";
constexpr const char* medium_prefix = "images/medium/";
constexpr const char* high_prefix = "images/high/";
constexpr const char* thumbnail_prefix = "images/thumbnail/";
constexpr const char* bucket = "verussensus";

struct quality {
    std::string prefix;
    int quality;
    int pixels;
};

const std::array<quality, 4> target_folders = {quality{thumbnail_prefix, 40, 800*800}, {low_prefix, 50, 1200*1080}, {medium_prefix, 75, 2560*1440}, {high_prefix, 90, -1}};

int main() {
    google::cloud::StatusOr<gcs::Client> client_status = gcs::Client::CreateDefaultClient().value();
    if (!client_status) {
        std::cerr << "Failed to create client: " << client_status.status() << "\n";
        return 1;
    }

    gcs::Client client = client_status.value();

    auto originals = client.ListObjects(bucket, gcs::Prefix{originals_prefix});
    for (auto& obj : originals) {
        if (!obj) {
            std::cerr << obj.status() << std::endl;
        }
        std::string original_name = obj.value().name();
        size_t original_size = obj.value().size();
        std::string filename = original_name.substr(std::string(originals_prefix).length());

        if (filename == "") {
            continue;
        }

        std::vector<unsigned char> original_buffer;
        std::vector<std::thread> threads;




        for (auto& target : target_folders) {
            auto prefix = target.prefix;
            int quality = target.quality;
            int pixels = target.pixels;
            auto name = prefix + filename;
            auto ok = client.GetObjectMetadata(bucket, name);
            if (ok) {
                continue;
            }

            std::cout << name << " is missing, creating" << std::endl;

            if (!original_buffer.size()) {
                auto reader = client.ReadObject(bucket, original_name);
                if (reader.bad()) {
                    std::cerr << reader.status() << std::endl;
                    return 1;
                }
                original_buffer.reserve(original_size);
                std::istreambuf_iterator<char> eos;
                auto it = std::istreambuf_iterator<char>{reader};
                while (it != eos && original_buffer.size() < original_size) {
                    int8_t value = *it++;
                    uint8_t tmp;
                    std::memcpy(&tmp, &value, sizeof(tmp));
                    original_buffer.push_back(tmp);
                }
                


            }

            std::thread t([&original_buffer, client, quality, name, pixels]() {
                std::vector<unsigned char> out(original_buffer.size());
                jpeg::optimize(std::as_const(original_buffer), out, quality, pixels);
                auto c = client.CreateDefaultClient();
                auto writer = c->WriteObject(bucket, name, gcs::ContentType{"image/jpeg"}, gcs::ContentEncoding{"binary"}, gcs::PredefinedAcl::PublicRead());
                std::ofstream file;
                for (auto& c : out) {
                    writer << c;
                }
                writer.Close();
            });

            threads.emplace_back(std::move(t));
        }
        for (auto& t : threads) {
            t.join();
        }

    }
    return 0;
}
