
PROJECT_DIR=$(realpath .)
BUILD_DIR=$(PROJECT_DIR)/build
SRC_DIR=$(PROJECT_DIR)/src
INCLUDE_DIR=$(PROJECT_DIR)/includes
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
INCLUDE_FILES := $(wildcard $(INCLUDE_FILES)/*.hpp)
AWS_LIBRARIES="s3"

.PHONY: install_dependencies
.PHONY: submodules
.PHONY: clean
.PHONY: build
.PHONY: toolchain

$(BUILD_DIR):
	mkdir -p build

install_dependencies:
	apt-get update
	apt-get install musl libcurl4-openssl-dev libssl-dev uuid-dev zlib1g-dev libpulse-dev

build: build/photominifier

build/photominifier: CMakeLists.txt $(SRC_FILES) $(INCLUDE_FILES) | $(BUILD_DIR) 
	cmake -H. -Bbuild -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release
	cmake --build build


submodules:
	git submodule update --init --recursive

mozjpeg: build/mozjpeg


toolchain: 
	./vcpkg/vcpkg install mozjpeg
	./vcpkg/vcpkg install google-cloud-cpp

vcpkg/vcpkg: submodules
	./vcpkg/bootstrap-vcpkg.sh -useSystemBinaries

clean:
	rm -rf build
