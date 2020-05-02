
PROJECT_DIR=$(realpath .)
BUILD_DIR=$(PROJECT_DIR)/build
SRC_DIR=$(PROJECT_DIR)/src
INCLUDE_DIR=$(PROJECT_DIR)/includes
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
INCLUDE_FILES := $(wildcard $(INCLUDE_FILES)/*.hpp)
AWS_LIBRARIES="s3"

LIBRARIES = aws-sdk-cpp mozjpeg

.PHONY: install_dependencies
.PHONY: submodules
.PHONY: clean
.PHONY: build

$(BUILD_DIR):
	mkdir -p build

install_dependencies:
	apt-get update
	apt-get install musl libcurl4-openssl-dev libssl-dev uuid-dev zlib1g-dev libpulse-dev

build: build/photominifier

build/photominifier: CMakeLists.txt $(SRC_FILES) $(INCLUDE_FILES) $(LIBRARIES) | $(BUILD_DIR) 
	cd build && cmake ${PROJECT_DIR} \
	-DCMAKE_INSTALL_PREFIX=$(BUILD_DIR)/install \
	&& $(MAKE) && $(MAKE) install

aws-sdk-cpp: build/aws-sdk-cpp

build/aws-sdk-cpp: | submodules $(BUILD_DIR)
	mkdir -p build/aws-sdk-cpp
	cd build/aws-sdk-cpp && cmake $(PROJECT_DIR)/aws-sdk-cpp \
	-DBUILD_ONLY=$(AWS_LIBRARIES) \
	-DBUILD_SHARED_LIBS=OFF \
	-DCMAKE_BUILD_TYPE=Release \
	-DCUSTOM_MEMORY_MANAGEMENT=OFF \
	-DCMAKE_INSTALL_PREFIX=$(BUILD_DIR)/install \
	-DENABLE_UNITY_BUILD=ON && $(MAKE) && $(MAKE) install

submodules:
	git submodule update --init --recursive

mozjpeg: build/mozjpeg

build/mozjpeg: | submodules $(BUILD_DIR)
	mkdir -p build/mozjpeg
	cd build/mozjpeg && cmake -G"Unix Makefiles" \
	 -DBUILD_SHARED_LIBS=OFF \
	 -DENABLE_SHARED=OFF \
	 -DPNG_SUPPORTED=OFF \
	 -DCMAKE_INSTALL_PREFIX=$(BUILD_DIR)/install \
	 -DREQUIRE_SIMD=TRUE \
	  $(PROJECT_DIR)/mozjpeg && make && make install

clean:
	rm -rf build
