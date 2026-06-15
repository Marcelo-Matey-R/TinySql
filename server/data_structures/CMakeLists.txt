add_library(data_structures
        src/BSTIndex.cpp
        src/BTreeIndex.cpp
        src/IndexFactory.cpp
)

target_include_directories(data_structures PUBLIC include/)