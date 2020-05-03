#include "s3.hpp"
#include "aws/core/Aws.h"
#include "aws/core/auth/AWSCredentialsProvider.h"
#include "aws/core/client/ClientConfiguration.h"
#include "aws/s3/S3Client.h"

#include "aws/s3/model/ListObjectsRequest.h"

#include <algorithm>


using namespace photorepository;


std::vector<Photograph> Repository::list_images() {
    Aws::S3::Model::ListObjectsRequest request;
    request.WithBucket("verussensus");
    auto resp = this->s3_client_.ListObjects(request);
    if (resp.IsSuccess()) {
        auto objects = resp.GetResult().GetContents();
        auto output = std::vector<Photograph>(objects.size());
        std::transform(objects.begin(), objects.end(), output.begin(), [](Aws::S3::Model::Object obj) {
            Photograph p;
            p.id = obj.GetKey();
            return p;
        });
        return output;
    } else {
        throw std::runtime_error(resp.GetError().GetMessage());
    }
    return {}; 
}

auto make_s3_client(std::string token, std::string secret, std::string endpoint) {
    Aws::InitAPI({});
    const auto credentials = Aws::Auth::SimpleAWSCredentialsProvider(token, secret).GetAWSCredentials();
    Aws::Client::ClientConfiguration config;
    config.endpointOverride = endpoint;
    auto c = Aws::S3::S3Client(credentials, config);
    return c;
}


Repository::Repository(std::string token, std::string secret, std::string endpoint) : s3_client_(make_s3_client(token, secret, endpoint)) {
    std::cout << "Hello" << std::endl;
}

Repository::Repository(std::string token, std::string secret) : Repository(token, secret, "https://fra1.digitaloceanspaces.com"){
    std::cout << "Hello" << std::endl;
}

Repository::~Repository() {
    Aws::ShutdownAPI({});
}