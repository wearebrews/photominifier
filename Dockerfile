FROM alpine:3.10 as build
WORKDIR /usr/app
RUN apk --update add \
 build-base \
 musl-dev \
 curl-dev \
 curl-static \
 cmake \
 git \
 zlib-dev \
 openssl-dev
COPY src src
COPY include include
COPY Makefile Makefile
COPY .git .git
COPY .gitmodules .gitmodules
COPY CMakeLists.txt CMakeLists.txt
RUN make build -B --jobs=4

FROM alpine:3.10
WORKDIR /usr/app 
COPY --from=build /usr/app/build/main main
RUN apk --update add libcurl libstdc++
CMD ["./main"] 