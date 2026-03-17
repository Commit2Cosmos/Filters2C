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
# Note, you will need to uncomment different files here as you start to add them to your project
add_executable(APImageFilters
    src/main.cpp
    #src/Filter.cpp
    #src/Image.cpp
    #src/Projection.cpp
    #src/Slice.cpp
    #src/Volume.cpp
    ${HEADER_FILES}
)

# Enable testing
include(CTest)
enable_testing()

# Include the tests
include(${CMAKE_SOURCE_DIR}/cmdtests.cmake)
