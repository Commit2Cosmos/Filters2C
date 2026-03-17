add_executable(EmbossTest
    test_edge.cpp
    test_emboss.cpp
    test_image.cpp
    test_projections.cpp
    test_sharpen.cpp
    test_simple_filters.cpp
    test_slice.cpp
    test_volume.cpp

    ${CMAKE_SOURCE_DIR}/src/EmbossFilter.cpp
    ${CMAKE_SOURCE_DIR}/src/ConvFilter.cpp
    ${CMAKE_SOURCE_DIR}/src/Image.cpp
)

target_include_directories(EmbossTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
set_target_properties(EmbossTest PROPERTIES CXX_STANDARD 17)

add_test(NAME EmbossTest COMMAND EmbossTest)
