set(PROTO_PATH "${CMAKE_CURRENT_SOURCE_DIR}/proto")
# optional for sqlite_pb_ext
# set(CUSTOM_SQLITE3 ON)
# set(SQLITE_VERSIO_NUMBER "3420000")


add_definitions(-DCMAKE_CURRENT_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/deps")
set(SQLITE3_PB_EXT_DIR ${CMAKE_CURRENT_BINARY_DIR}/deps/sqlite3_pb_ext)
if(NOT EXISTS ${SQLITE3_PB_EXT_DIR})
    message("Set sqlite3_pb_ext_directory: ${SQLITE3_PB_EXT_DIR}")
    execute_process(
        COMMAND "sqlite3-pb-ext-gen" --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/proto --out_path=${SQLITE3_PB_EXT_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto
        RESULT_VARIABLE RESULT
    )
    if(NOT RESULT EQUAL 0)
        message(FATAL_ERROR "Command failed: sqlite3-pb-ext-gen --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/proto -o${SQLITE3_PB_EXT_DIR} *.proto")
    endif()
endif()

add_subdirectory(${SQLITE3_PB_EXT_DIR} sqlite3_pb_ext)
