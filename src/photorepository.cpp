#include "s3.hpp"
#include "aws/core/Aws.h"
#include "aws/core/auth/AWSCredentialsProvider.h"
#include "aws/core/client/ClientConfiguration.h"
#include "aws/s3/S3Client.h"
#include <fstream>

#include "aws/s3/model/ListObjectsRequest.h"
#include "aws/s3/model/GetObjectRequest.h"

#include <algorithm>
#include <tuple>


#include <experimental/filesystem>
#include <streambuf>


using namespace photorepository;

constexpr const char* BUCKET = "verussensus";


std::vector<Photograph> ListPhotographs(std::string prefix, const Aws::S3::S3Client& client) {
    Aws::S3::Model::ListObjectsRequest request;
    request.WithBucket(BUCKET)
           .WithPrefix(prefix);
    auto resp = client.ListObjects(request);
    if (resp.IsSuccess()) {
        auto objects = resp.GetResult().GetContents();
        auto output = std::vector<Photograph>(objects.size());
        std::transform(objects.begin(), objects.end(), output.begin(), [](Aws::S3::Model::Object obj) {
            return Photograph(obj.GetKey());
        });
        return output;
    } else {
        throw std::runtime_error(resp.GetError().GetMessage());
    }
    return {}; 
}

std::vector<Photograph> Repository::List(std::string prefix) {
    return ListPhotographs(prefix, this->s3_client_);
}

const std::vector<char> Repository::Download(const Photograph& photo) {
    namespace fs =  std::experimental::filesystem;
    Aws::S3::Model::GetObjectRequest request;
    request.WithBucket(BUCKET).WithKey(photo.FileName());
    auto outcome = this->s3_client_.GetObject(request);
    if (!outcome.IsSuccess()) {
        throw std::runtime_error(outcome.GetError().GetMessage());
    }
    auto retrieved_file = outcome.GetResultWithOwnership();
    auto size = retrieved_file.GetContentLength();
    std::vector<char> buf;
    buf.reserve(size);
    auto* stream = retrieved_file.GetBody().rdbuf();
    buf.insert(buf.begin(), std::istreambuf_iterator(stream), std::istreambuf_iterator<char>());
    return buf;
}



auto make_s3_client(std::string token, std::string secret, std::string endpoint) {
    Aws::InitAPI({});
    const auto credentials = Aws::Auth::SimpleAWSCredentialsProvider(token, secret).GetAWSCredentials();
    Aws::Client::ClientConfiguration config;
    config.endpointOverride = endpoint;
    return Aws::S3::S3Client(credentials, config);
}


Repository::Repository(std::string token, std::string secret, std::string endpoint) : s3_client_(make_s3_client(token, secret, endpoint)) {}

Repository::~Repository() {
    Aws::ShutdownAPI({});
}