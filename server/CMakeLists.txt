cmake_minimum_required(VERSION 3.16)
project(TinySQLDb)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(data_structures)
add_subdirectory(stored_data_manager)
add_subdirectory(query_processor)
add_subdirectory(api)