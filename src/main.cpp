#include <iostream>
#include <stdlib.h>
#include "aws/core/Aws.h"
#include "aws/core/auth/AWSCredentials.h"
#include "aws/core/auth/AWSCredentialsProvider.h"
#include "aws/core/client/ClientConfiguration.h"
#include "aws/s3/S3Client.h"
#include "aws/s3/model/ListObjectsRequest.h"

#include "curl/curl.h"
#include "s3.hpp"

extern "C" {
    #include <jpeglib.h>
}

const auto TOKEN = getenv("TOKEN");
const auto SECRET = getenv("SECRET");
const auto ENDPOINT_URL = "https://fra1.digitaloceanspaces.com";
const auto CREDENTIALS = Aws::Auth::SimpleAWSCredentialsProvider(TOKEN, SECRET).GetAWSCredentials();

int main() {
    photorepository::Repository rep;
    auto photos = rep.list_images();
    for (auto p : photos) {
        std::cout << p.file_name << std::endl;
    }
    Aws::SDKOptions options;
    Aws::InitAPI(options);
    Aws::Client::ClientConfiguration client_config;
    client_config.endpointOverride = ENDPOINT_URL;
    auto s3_client = Aws::S3::S3Client(CREDENTIALS, client_config);  
    auto result = s3_client.ListBuckets();
    if (result.IsSuccess()) {
        std::cout << "Buckets:" << std::endl;
        for (const auto &bucket : result.GetResult().GetBuckets()) {
            std::cout << "- " << bucket.GetName() << std::endl;
        }
    } else {
        std::cout << result.GetError() << std::endl;
    }


    Aws::ShutdownAPI(options);
    return 0;
}