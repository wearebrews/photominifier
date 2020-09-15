#include <iostream>
#include <stdlib.h>
#include "curl/curl.h"
#include <memory>
#include "google/cloud/storage/client.h"
#include <fstream>
#include "optimize_jpeg.hpp"
#include <thread>


namespace gcs = google::cloud::storage;

int main() {
    google::cloud::StatusOr<gcs::Client> client = gcs::Client::CreateDefaultClient();
    if (!client) {
        std::cerr << "Failed to create client: " << client.status() << "\n";
        return 1;
    }

    auto list = client->ListBucketsForProject("wearebrews");
    for (auto& b : list) {
        auto objects = client->ListObjects(b.value().name(), gcs::Prefix("images/fullsize"));
        for (auto& o : objects) {
            std::cout << o.value().name() << std::endl;
        }
    }
    return 0;
}
