add_executable(api
        src/main.cpp
        src/ApiServer.cpp
)

target_include_directories(api PRIVATE include/)

target_link_libraries(api PRIVATE query_processor)