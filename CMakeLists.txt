cmake_minimum_required(VERSION 3.16)

project(foxruler VERSION 0.7 LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "In-source builds are not supported; use: cmake -S . -B build")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(FOX 2.0 REQUIRED)

add_executable(foxruler
  src/main.cpp
  src/rulericon.cpp)

target_link_libraries(foxruler PRIVATE FOX::FOX)

if(MSVC)
  target_compile_options(foxruler PRIVATE /W3)
else()
  target_compile_options(foxruler PRIVATE -Wall -Wextra)
endif()

install(TARGETS foxruler RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
