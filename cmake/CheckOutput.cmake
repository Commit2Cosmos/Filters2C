# CheckOutput.cmake

# Split the space-separated ARGS string into CMake list
separate_arguments(ARGS_LIST UNIX_COMMAND "${ARGS}")

execute_process(
    COMMAND ${EXE} ${ARGS_LIST}
    RESULT_VARIABLE CMD_RESULT
)

if(NOT CMD_RESULT EQUAL 0)
    message(FATAL_ERROR "Command failed (exit ${CMD_RESULT})")
endif()

if(NOT EXISTS "${OUTPUT_FILE}")
    message(FATAL_ERROR "Output file not created: ${OUTPUT_FILE}")
endif()