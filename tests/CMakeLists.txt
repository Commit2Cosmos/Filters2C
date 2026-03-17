# Helper to reduce repetition
set(TEST_SOURCES
    test_blur
    test_edge
    test_emboss
    test_image
    test_projections
    test_sharpen
    test_simple_filters
    test_slice
    test_volume
)

foreach(TEST_NAME IN LISTS TEST_SOURCES)
    add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
    target_link_libraries(${TEST_NAME} PRIVATE APFiltersLib)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
