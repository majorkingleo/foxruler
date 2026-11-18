#!/usr/bin/env bash
#
# Configure and build foxruler with CMake.
#
# Usage: ./build.sh [options]
#   --debug        build with debug symbols and no optimisation
#   --clean        remove the build directory before configuring
#   --run          run the application after a successful build
#   --install      install to CMAKE_INSTALL_PREFIX (default /usr/local)
#   -h, --help     show this help
#
# FOX_ROOT may point at a FOX Toolkit installation prefix, for example:
#   FOX_ROOT=/opt/fox ./build.sh

set -euo pipefail

readonly project_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
readonly build_dir="${project_dir}/build"

build_type="Release"
clean=0
run=0
install=0

usage(){
  sed -n '3,13p' "${BASH_SOURCE[0]}" | sed 's/^# \{0,1\}//'
}

while [ $# -gt 0 ]; do
  case "$1" in
    --debug) build_type="Debug" ;;
    --clean) clean=1 ;;
    --run) run=1 ;;
    --install) install=1 ;;
    -h|--help) usage; exit 0 ;;
    *) echo "Unknown option: $1" >&2; usage >&2; exit 2 ;;
  esac
  shift
done

cmake_args=(
  -S "${project_dir}"
  -B "${build_dir}"
  -DCMAKE_BUILD_TYPE="${build_type}"
)

if [ -n "${FOX_ROOT:-}" ]; then
  cmake_args+=(-DFOX_ROOT="${FOX_ROOT}")
fi

if [ "${clean}" -eq 1 ]; then
  rm -rf "${build_dir}"
fi

cmake "${cmake_args[@]}"
cmake --build "${build_dir}" --parallel "$(nproc 2>/dev/null || echo 2)"

if [ "${install}" -eq 1 ]; then
  cmake --install "${build_dir}"
fi

if [ "${run}" -eq 1 ]; then
  exec "${build_dir}/foxruler" "$@"
fi
