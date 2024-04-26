cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(sqlite_pb_ext_example LANGUAGES CXX)

include(sqlite3_pb_ext_deps.cmake "addressbook.proto")

add_executable(sqlite_pb_ext_example src/main.cpp ${SQLITE3_PB_EXT_DIR}/src/gen/pb/addressbook.pb.h ${SQLITE3_PB_EXT_DIR}/src/gen/pb/addressbook.pb.cc)
add_dependencies(sqlite_pb_ext_example sqlite3_pb_ext)
target_link_libraries(sqlite_pb_ext_example PRIVATE sqlite3_pb_ext )
include_directories("${CMAKE_CURRENT_BINARY_DIR}/deps/sqlite3_pb_ext/src/gen/pb")
