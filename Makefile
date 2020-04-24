
PROJECT_FOLDER=$(realpath .)
LIBS_FOLDER=$(PROJECT_FOLDER)/libs
AWS_LIBRARIES="s3"

install_dependencies:
	apt-get update
	apt-get install musl libcurl4-openssl-dev libssl-dev uuid-dev zlib1g-dev libpulse-dev

build: libs/aws/
	mkdir -p build
	cd build && cmake .. \
	-DAWSSDK_DIR=libs/aws-sdk-cpp \
	-DCMAKE_INSTALL_PREFIX=$(LIBS_FOLDER) \
	-DBUILD_SHARED_LIBS=OFF && $(MAKE)

.PHONY: build

libs/aws/: submodules
	mkdir -p aws-sdk-cpp/build
	cd aws-sdk-cpp/build && cmake .. \
	-DBUILD_ONLY=$(AWS_LIBRARIES) \
	-DBUILD_SHARED_LIBS=OFF \
	-DCMAKE_BUILD_TYPE=Release \
	-DCUSTOM_MEMORY_MANAGEMENT=OFF \
	-DCMAKE_INSTALL_PREFIX=$(LIBS_FOLDER) \
	-DENABLE_UNITY_BUILD=ON && $(MAKE) && $(MAKE) install

submodules:
	git submodule update --init --recursive
.PHONY: submodules



lib/mozjpeg: submodules
	mkdir -p build/mozjpeg
	cd build/mozjpeg && cmake -G"Unix Makefiles" \
	 -DBUILD_SHARED_LIBS=OFF \
	 -DCMAKE_INSTALL_PREFIX=$(LIBS_FOLDER) \
	  $(PROJECT_FOLDER)/mozjpeg && make && make install

