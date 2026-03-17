cmake_minimum_required(VERSION 3.21)

# Project name and version
project(APImageFilters VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Define the header files location
file(GLOB_RECURSE HEADER_FILES ${CMAKE_SOURCE_DIR}/src/*.h)

# Add the executable
add_executable(APImageFilters
    src/main.cpp
    src/Blur.cpp
    src/Brightness.cpp
    src/ConvFilter.cpp
    src/EmbossFilter.cpp
    src/GrayScale.cpp
    src/HistogramEqualisation.cpp
    src/Image.cpp
    src/PrewittFilter.cpp
    src/Projection.cpp
    src/RobertsCrossFilter.cpp
    src/SaltAndPepper.cpp
    src/ScharrFilter.cpp
    src/SharpenFilter.cpp
    src/Slice.cpp
    src/SobelFilter.cpp
    src/Threshold.cpp
    src/Volume.cpp
    ${HEADER_FILES}
)

# Enable testing
include(CTest)
enable_testing()

# Include the tests
add_subdirectory(tests)
include(${CMAKE_SOURCE_DIR}/cmdtests.cmake)
