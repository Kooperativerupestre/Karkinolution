FROM ubuntu:25.04 AS development

RUN apt-get update && apt-get install -y \
    clang-20 \
    clang++-20 \
    gcc-15 \
    g++-15 \
    cmake \
    ninja-build \
    ccache \
    git \
    && rm -rf /var/lib/apt/lists/*

ENV CC=clang-20
ENV CXX=clang++-20

WORKDIR /workspace

COPY . /workspace

RUN cmake \
    -S /workspace \
    -B /workspace/build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_COMPILER=clang++-20 \
    -DCMAKE_C_COMPILER=clang-20

RUN cmake --build /workspace/build


FROM development AS test

WORKDIR /workspace

CMD ["ctest", "--test-dir", "/workspace/build", "--output-on-failure"]