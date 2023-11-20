cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(sqlite_pb_ext_example LANGUAGES CXX)

include(sqlite3_pb_ext_deps.cmake)

add_executable(sqlite_pb_ext_example src/main.cpp proto/generated_from_sqlite3_pb_ext/addressbook.pb.h proto/generated_from_sqlite3_pb_ext/addressbook.pb.cc)
add_dependencies(sqlite_pb_ext_example sqlite3_pb_ext)
target_link_libraries(sqlite_pb_ext_example PRIVATE sqlite3_pb_ext )
