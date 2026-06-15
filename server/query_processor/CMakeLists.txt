add_library(query_processor
        src/Lexer.cpp
        src/Parser.cpp
        src/SemanticValidator.cpp
        src/QueryExecutor.cpp
)

target_include_directories(query_processor PUBLIC include/)

target_link_libraries(query_processor PRIVATE stored_data_manager)