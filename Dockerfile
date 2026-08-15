FROM development AS test

COPY . /workspace

RUN cmake \
    -S /workspace \
    -B /workspace/build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_C_COMPILER=clang

RUN cmake --build /workspace/build

CMD ["ctest", "--test-dir", "/workspace/build", "--output-on-failure"]