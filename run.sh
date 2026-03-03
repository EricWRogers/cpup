#!/bin/bash
set -euo pipefail

BUILD_DIR="${BUILD_DIR:-build-dev}"
BUILD_TYPE="${BUILD_TYPE:-Debug}"
JOBS="${JOBS:-$(nproc)}"

cmake -S . -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
cmake --build "${BUILD_DIR}" -j"${JOBS}"
./fogpi
