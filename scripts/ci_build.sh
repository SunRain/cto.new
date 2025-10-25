#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="${BUILD_DIR:-build}"

cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" --parallel

pushd "$BUILD_DIR" > /dev/null
ctest --output-on-failure || true
popd > /dev/null

qmltestrunner -input "$(pwd)/tests/qmltests"
