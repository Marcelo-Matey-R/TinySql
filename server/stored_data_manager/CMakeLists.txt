add_library(stored_data_manager
        src/StorageManager.cpp
        src/SystemCatalog.cpp
        src/TableFile.cpp
        src/Encryptor.cpp
)

target_include_directories(stored_data_manager PUBLIC include/)

target_link_libraries(stored_data_manager PRIVATE data_structures)