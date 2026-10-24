FROM ubuntu:26.04 AS development

RUN apt-get update && apt-get install -y \
    clang-21 \
    clang++-21 \
    gcc-16 \
    g++-16 \
    cmake \
    ninja-build \
    ccache \
    git \
    && rm -rf /var/lib/apt/lists/*

ENV CC=clang-21
ENV CXX=clang++-21

WORKDIR /workspace

COPY . /workspace

RUN cmake \
    -S /workspace \
    -B /workspace/build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_COMPILER=clang++-21 \
    -DCMAKE_C_COMPILER=clang-21 \
    -DCMAKE_CXX_FLAGS="-stdlib=libstdc++ --gcc-toolchain=/usr"

RUN clang++-21 -std=c++26 -E -x c++ /dev/null -v 2>&1 | grep -F 'Selected GCC installation: /usr/lib/gcc/x86_64-linux-gnu/16' && \
    test -n "$(find /usr/include/c++ -name inplace_vector -print -quit)" && \
    printf '#include <inplace_vector>\nint main() { std::inplace_vector<int, 1> values; }\n' | \
        clang++-21 --gcc-toolchain=/usr -stdlib=libstdc++ -std=c++26 -x c++ -fsyntax-only -

RUN cmake --build /workspace/build


FROM development AS test

WORKDIR /workspace

CMD ["ctest", "--test-dir", "/workspace/build", "--output-on-failure"]
