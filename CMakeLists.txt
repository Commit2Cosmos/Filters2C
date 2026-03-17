cmake_minimum_required(VERSION 3.21)
project(APImageFilters VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE HEADER_FILES ${CMAKE_SOURCE_DIR}/src/*.h)

add_library(APFiltersLib STATIC
    src/BoxBlurFilter.cpp
    src/Brightness.cpp
    src/ConvFilter.cpp
    src/EmbossFilter.cpp
    src/GaussianBlurFilter.cpp
    src/GrayScale.cpp
    src/HistogramEqualisation.cpp
    src/Image.cpp
    src/MedianBlurFilter.cpp
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
target_include_directories(APFiltersLib PUBLIC ${CMAKE_SOURCE_DIR}/src)

add_executable(APImageFilters src/main.cpp)
target_link_libraries(APImageFilters PRIVATE APFiltersLib)

enable_testing()
add_subdirectory(tests)
include(${CMAKE_SOURCE_DIR}/cmdtests.cmake)