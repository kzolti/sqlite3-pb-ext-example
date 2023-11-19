cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(sqlite_pb_ext_example LANGUAGES CXX)

# required for sqlite_pb_ext
set(PROTO_PATH "${CMAKE_CURRENT_SOURCE_DIR}/proto")
# optional for sqlite_pb_ext
#set(CUSTOM_SQLITE3 ON)
#set(SQLITE_VERSIO_NUMBER "3420000")

find_package(SQLite3 REQUIRED)
if(SQLite3_FOUND)
    message(STATUS "Found SQLite3  ${SQLite3_VERSION}\t ${SQLite3_LIBRARIES}")
    include_directories(${SQLite3_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "SQLite3 not found")
endif()
find_package(Protobuf REQUIRED)
if(Protobuf_FOUND)
    message(STATUS "Found Protobuf ${Protobuf_VERSION}\t ${Protobuf_LIBRARIES}")
    include_directories(${Protobuf_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Protobuf not found")
endif()
add_definitions(-DCMAKE_CURRENT_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/deps")
set(SQLITE3_PB_EXT_DIR ${CMAKE_CURRENT_BINARY_DIR}/deps/sqlite3_pb_ext)
if(NOT EXISTS ${SQLITE3_PB_EXT_DIR})
    message("Set sqlite3_pb_ext_directory: ${SQLITE3_PB_EXT_DIR}")
    execute_process(
        COMMAND "sqlite3_pb_ext_gen" --proto_path=${PROTO_PATH} --out_path=${SQLITE3_PB_EXT_DIR}
        RESULT_VARIABLE RESULT
    )
    if(NOT RESULT EQUAL 0)
        message(FATAL_ERROR "Command failed: sqlite3_pb_ext_gen -I${PROTO_PATH} -o${SQLITE3_PB_EXT_DIR}")
    endif()
endif()


add_subdirectory(${SQLITE3_PB_EXT_DIR} sqlite3_pb_ext)

add_executable(sqlite_pb_ext_example src/main.cpp proto/generate_from_sqlite3_pb_ext/addressbook.pb.h proto/generate_from_sqlite3_pb_ext/addressbook.pb.cc)
add_dependencies(sqlite_pb_ext_example sqlite3_pb_ext)
target_link_libraries(sqlite_pb_ext_example sqlite3_pb_ext ${Protobuf_LIBRARIES} ${SQLite3_LIBRARIES})

