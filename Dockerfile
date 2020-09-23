FROM haavardm/google-cloud-cpp:alpine-3.10 as build

RUN apk add nasm
RUN vcpkg/vcpkg install mozjpeg --triplet x64-linux

WORKDIR app

COPY src src
COPY include include
COPY .gitmodules .gitmodules
COPY CMakeLists.txt CMakeLists.txt
RUN cmake -H. -Bbuild -GNinja -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build

FROM alpine:3.10
WORKDIR app
COPY --from=build /app/build/photominifier main

RUN apk update && apk add ca-certificates && rm -rf /var/cache/apk/*

CMD ["./main"]