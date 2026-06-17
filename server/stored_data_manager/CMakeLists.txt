add_library(stored_data_manager
        src/StorageManager.cpp
        src/SystemCatalog.cpp
        src/TableFile.cpp
        src/Encryptor.cpp
        src/PathUtils.cpp
        include/PathUtils.h
)

target_include_directories(stored_data_manager PUBLIC include/)

target_link_libraries(stored_data_manager PRIVATE data_structures)