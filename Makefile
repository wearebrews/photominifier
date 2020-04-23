
INSTALL_FOLDER=$(realpath .)
AWS_LIBRARIES="s3"

install_dependencies:
	apt-get update
	apt-get install musl libcurl4-openssl-dev libssl-dev uuid-dev zlib1g-dev libpulse-dev

build: include/aws
	mkdir -p build
	cd build && cmake .. \
	-DBUILD_SHARED_LIBS=OFF \
	-DCMAKE_INSTALL_PREFIX=$(INSTALL_FOLDER) && $(MAKE)

.PHONY: build

include/aws: 
	if cd build/aws-sdk-cpp; then git pull; else git clone https://github.com/aws/aws-sdk-cpp.git build/aws-sdk-cpp; fi
	mkdir -p build/aws-sdk-cpp/build
	cd build/aws-sdk-cpp/build && cmake .. \
	-DBUILD_ONLY=$(AWS_LIBRARIES) \
	-DBUILD_SHARED_LIBS=OFF \
	-DCMAKE_BUILD_TYPE=Release \
	-DCUSTOM_MEMORY_MANAGEMENT=OFF \
	-DCMAKE_INSTALL_PREFIX=$(INSTALL_FOLDER) \
	-DENABLE_UNITY_BUILD=ON && $(MAKE) && $(MAKE) install

