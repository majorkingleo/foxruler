# foxruler

FoxRuler is a simple pixel ruler based on [FOX Toolkit](http://fox-toolkit.org/).

![Screenshot](https://raw.github.com/majorkingleo/foxruler/main/screenshot.png)

## Requirements

- CMake 3.16 or newer
- A C++17 compiler
- FOX Toolkit 2.x

## Build

```sh
./build.sh
```

`build.sh` configures a `build/` directory with CMake and compiles the
application. If FOX is installed outside the default search paths, point
`FOX_ROOT` at the installation prefix:

```sh
FOX_ROOT=/opt/fox ./build.sh
```

Build options:

```sh
./build.sh --debug    # debug build
./build.sh --clean    # remove build/ before configuring
./build.sh --run      # run foxruler after the build
./build.sh --install  # install with cmake --install
```

The same steps can be run directly:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

## Project layout

```text
cmake/FindFOX.cmake   CMake package lookup for FOX 2.x
src/main.cpp          application and main window
CMakeLists.txt
build.sh
```
