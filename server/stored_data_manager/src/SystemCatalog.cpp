//
// Created by josueaperez on 6/14/26.
//

#include "SystemCatalog.h"
#include "PathUtils.h"
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <cstring>

namespace fs = std::filesystem;

// tamanio fijo para los campos
static const int NAME_SIZE = 64;
static const int TYPE_SIZE = 16;
static const int MAX_COLUMNS = 32;

// la parte del singleton
SystemCatalog::SystemCatalog() {
    catalogPath = PathUtils::getExecutableDir() + "/data/system_catalog/";
    fs::create_directories(catalogPath);
}

SystemCatalog& SystemCatalog::getInstance() {
    static SystemCatalog instance;
    return instance;
}

// los helpers
void SystemCatalog::writeString(std::ofstream& file,
                                 const std::string& str, int size) {
    char buffer[size];
    memset(buffer, 0, size);
    strncpy(buffer, str.c_str(), size - 1);
    file.write(buffer, size);
}

std::string SystemCatalog::readString(std::ifstream& file, int size) {
    char buffer[size];
    file.read(buffer, size);
    return std::string(buffer);
}

// la parte de bases de datos
void SystemCatalog::addDatabase(const std::string& dbName) {
    std::ofstream file(catalogPath + "SystemDatabases.bin",
                       std::ios::binary | std::ios::app);
    if (!file) throw std::runtime_error("Cannot open SystemDatabases.bin");
    writeString(file, dbName, NAME_SIZE);
}

bool SystemCatalog::databaseExists(const std::string& dbName) {
    std::ifstream file(catalogPath + "SystemDatabases.bin", std::ios::binary);
    if (!file) return false;

    while (file.peek() != EOF) {
        std::string name = readString(file, NAME_SIZE);
        if (name == dbName) return true;
    }
    return false;
}

std::vector<std::string> SystemCatalog::getAllDatabases() {
    std::vector<std::string> databases;
    std::ifstream file(catalogPath + "SystemDatabases.bin", std::ios::binary);
    if (!file) return databases;

    while (file.peek() != EOF) {
        std::string name = readString(file, NAME_SIZE);
        if (!name.empty()) databases.push_back(name);
    }
    return databases;
}

// la parte de las tablas
// la estructura que va a tener al estar en el disco es:
// Estructura en disco para SystemTables:
// [dbName: 64][tableName: 64][numColumns: 4][deleted: 1]
// Estructura en disco para SystemColumns:
// [dbName: 64][tableName: 64][columnName: 64][type: 16][size: 4]

void SystemCatalog::addTable(const std::string& dbName,
                              const std::string& tableName,
                              const std::vector<std::string>& columns,
                              const std::vector<std::string>& types,
                              const std::vector<int>& sizes) {
    // escribir en SystemTables
    std::ofstream tablesFile(catalogPath + "SystemTables.bin",
                             std::ios::binary | std::ios::app);
    if (!tablesFile) throw std::runtime_error("Cannot open SystemTables.bin");

    writeString(tablesFile, dbName, NAME_SIZE);
    writeString(tablesFile, tableName, NAME_SIZE);
    int numColumns = (int)columns.size();
    tablesFile.write(reinterpret_cast<char*>(&numColumns), sizeof(int));
    char deleted = 0;
    tablesFile.write(&deleted, 1);

    // escribir en SystemColumns
    std::ofstream colsFile(catalogPath + "SystemColumns.bin",
                           std::ios::binary | std::ios::app);
    if (!colsFile) throw std::runtime_error("Cannot open SystemColumns.bin");

    for (int i = 0; i < (int)columns.size(); i++) {
        writeString(colsFile, dbName, NAME_SIZE);
        writeString(colsFile, tableName, NAME_SIZE);
        writeString(colsFile, columns[i], NAME_SIZE);
        writeString(colsFile, types[i], TYPE_SIZE);
        colsFile.write(reinterpret_cast<const char*>(&sizes[i]), sizeof(int));
    }
}

void SystemCatalog::removeTable(const std::string& dbName,
                                 const std::string& tableName) {
    // marca como eliminada en SystemTables
    std::fstream file(catalogPath + "SystemTables.bin",
                      std::ios::binary | std::ios::in | std::ios::out);
    if (!file) return;

    int recordSize = NAME_SIZE + NAME_SIZE + sizeof(int) + 1;

    while (file.peek() != EOF) {
        std::streampos pos = file.tellg();
        char dbBuf[NAME_SIZE], tableBuf[NAME_SIZE];
        file.read(dbBuf, NAME_SIZE);
        file.read(tableBuf, NAME_SIZE);
        int numCols;
        file.read(reinterpret_cast<char*>(&numCols), sizeof(int));
        char deleted;
        file.read(&deleted, 1);

        if (std::string(dbBuf) == dbName &&
            std::string(tableBuf) == tableName) {
            file.seekp(pos + (std::streamoff)(recordSize - 1));
            char d = 1;
            file.write(&d, 1);
            return;
            }
    }
}

bool SystemCatalog::tableExists(const std::string& dbName,
                                 const std::string& tableName) {
    std::ifstream file(catalogPath + "SystemTables.bin", std::ios::binary);
    if (!file) return false;

    while (file.peek() != EOF) {
        char dbBuf[NAME_SIZE], tableBuf[NAME_SIZE];
        file.read(dbBuf, NAME_SIZE);
        file.read(tableBuf, NAME_SIZE);
        int numCols;
        file.read(reinterpret_cast<char*>(&numCols), sizeof(int));
        char deleted;
        file.read(&deleted, 1);

        if (std::string(dbBuf) == dbName &&
            std::string(tableBuf) == tableName && deleted == 0)
            return true;
    }
    return false;
}

std::vector<std::string> SystemCatalog::getTablesForDatabase(
    const std::string& dbName) {
    std::vector<std::string> tables;
    std::ifstream file(catalogPath + "SystemTables.bin", std::ios::binary);
    if (!file) return tables;

    while (file.peek() != EOF) {
        char dbBuf[NAME_SIZE], tableBuf[NAME_SIZE];
        file.read(dbBuf, NAME_SIZE);
        file.read(tableBuf, NAME_SIZE);
        int numCols;
        file.read(reinterpret_cast<char*>(&numCols), sizeof(int));
        char deleted;
        file.read(&deleted, 1);

        if (std::string(dbBuf) == dbName && deleted == 0)
            tables.push_back(std::string(tableBuf));
    }
    return tables;
}

//la parte de las columnas

TableMeta SystemCatalog::getTableMeta(const std::string& dbName,
                                       const std::string& tableName) {
    TableMeta meta;
    meta.recordSize = 0;

    std::ifstream file(catalogPath + "SystemColumns.bin", std::ios::binary);
    if (!file) throw std::runtime_error("Cannot open SystemColumns.bin");

    while (file.peek() != EOF) {
        char dbBuf[NAME_SIZE], tableBuf[NAME_SIZE];
        char colBuf[NAME_SIZE], typeBuf[TYPE_SIZE];
        int size;

        file.read(dbBuf, NAME_SIZE);
        file.read(tableBuf, NAME_SIZE);
        file.read(colBuf, NAME_SIZE);
        file.read(typeBuf, TYPE_SIZE);
        file.read(reinterpret_cast<char*>(&size), sizeof(int));

        if (std::string(dbBuf) == dbName &&
            std::string(tableBuf) == tableName) {
            meta.columns.push_back(std::string(colBuf));
            meta.types.push_back(std::string(typeBuf));
            meta.sizes.push_back(size);
            meta.recordSize += size;
            }
    }

    if (meta.columns.empty())
        throw std::runtime_error("Table not found: " + dbName + "." + tableName);

    // se agrega 1 byte para el flag de deleted
    meta.recordSize += 1;
    return meta;
}

int SystemCatalog::getColumnIndex(const std::string& dbName,
                                   const std::string& tableName,
                                   const std::string& columnName) {
    TableMeta meta = getTableMeta(dbName, tableName);
    for (int i = 0; i < (int)meta.columns.size(); i++)
        if (meta.columns[i] == columnName) return i;
    return -1;
}

// la parte de los indices
// la estructura en disco para SystemIndexes es:
//[dbName: 64][tableName: 64][columnName: 64][indexName: 64][type: 16]

void SystemCatalog::addIndex(const std::string& dbName,
                              const std::string& tableName,
                              const std::string& columnName,
                              const std::string& indexName,
                              const std::string& type) {
    std::ofstream file(catalogPath + "SystemIndexes.bin",
                       std::ios::binary | std::ios::app);
    if (!file) throw std::runtime_error("Cannot open SystemIndexes.bin");

    writeString(file, dbName, NAME_SIZE);
    writeString(file, tableName, NAME_SIZE);
    writeString(file, columnName, NAME_SIZE);
    writeString(file, indexName, NAME_SIZE);
    writeString(file, type, TYPE_SIZE);
}

std::vector<IndexInfo> SystemCatalog::getAllIndexes() {
    std::vector<IndexInfo> indexes;
    std::ifstream file(catalogPath + "SystemIndexes.bin", std::ios::binary);
    if (!file) return indexes;

    while (file.peek() != EOF) {
        IndexInfo idx;
        idx.dbName     = readString(file, NAME_SIZE);
        idx.tableName  = readString(file, NAME_SIZE);
        idx.columnName = readString(file, NAME_SIZE);
        idx.indexName  = readString(file, NAME_SIZE);
        idx.type       = readString(file, TYPE_SIZE);
        if (!idx.dbName.empty()) indexes.push_back(idx);
    }
    return indexes;
}

bool SystemCatalog::indexExists(const std::string& dbName,
                                 const std::string& tableName,
                                 const std::string& columnName) {
    auto indexes = getAllIndexes();
    for (auto& idx : indexes)
        if (idx.dbName == dbName && idx.tableName == tableName &&
            idx.columnName == columnName) return true;
    return false;
}
