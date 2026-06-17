//
// Created by josueaperez on 6/16/26.
//

#include "PathUtils.h"
#include <filesystem>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <limits.h>
#endif

std::string PathUtils::getExecutableDir() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return std::filesystem::path(buffer).parent_path().string();
#else
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) buffer[len] = '\0';
    return std::filesystem::path(buffer).parent_path().string();
#endif
}
