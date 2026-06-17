//
// Created by josueaperez on 6/14/26.
//

#include "StorageManager.h"
#include "PathUtils.h"
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace fs = std::filesystem;

// La parte del singleton

StorageManager::StorageManager() {
    dataPath = PathUtils::getExecutableDir() + "/data/";
    fs::create_directories(dataPath + "system_catalog");
    loadIndexes();
}

StorageManager &StorageManager::getInstance() {
    static StorageManager instance;
    return instance;
}

// La parte de la base de datos

bool StorageManager::createDatabase(const std::string &dbName) {
    if (databaseExists(dbName))
        return false;
    fs::create_directories(dataPath + dbName);
    SystemCatalog::getInstance().addDatabase(dbName);
    return true;
}

bool StorageManager::databaseExists(const std::string &dbName) {
    return fs::exists(dataPath + dbName) && fs::is_directory(dataPath + dbName);
}

//La parte de las tablas

bool StorageManager::createTable(const std::string &dbName, const std::string &tableName,
                                 const std::vector<std::string> &columns, const std::vector<std::string> &types,
                                 const std::vector<int> &sizes) {
    if (!databaseExists(dbName) || tableExists(dbName, tableName))
        return false;
    std::string path = dataPath + dbName + "/" + tableName + ".bin";
    TableFile::create(path, columns, types, sizes);
    SystemCatalog::getInstance().addTable(dbName, tableName, columns, types, sizes);
    return true;
}

bool StorageManager::dropTable(const std::string &dbName, const std::string &tableName) {
    if (!tableExists(dbName, tableName) || !tableIsEmpty(dbName, tableName))
        return false;

    std::string path = dataPath + dbName + "/" + tableName + ".bin";
    fs::remove(path);
    SystemCatalog::getInstance().removeTable(dbName, tableName);
    return true;
}

bool StorageManager::tableExists(const std::string &dbName, const std::string &tableName) {
    std::string path = dataPath + dbName + "/" + tableName + ".bin";
    return fs::exists(path);
}

bool StorageManager::tableIsEmpty(const std::string &dbName, const std::string &tableName) {
    std::string path = dataPath + dbName + "/" + tableName + ".bin";
    return TableFile::isEmpty(path);
}

// parte de registros

int StorageManager::insertRecord(const std::string &dbName,
                                 const std::string &tableName,
                                 const std::vector<std::string> &values) {
    std::string path = dataPath + dbName + "/" + tableName + ".bin";
    auto meta = SystemCatalog::getInstance().getTableMeta(dbName, tableName);

    for (int i = 0; i < (int) meta.columns.size(); i++) {
        std::string indexKey = getIndexKey(dbName, tableName, meta.columns[i]);
        if (indexes.count(indexKey)) {
            int existing = indexes[indexKey]->search(values[i]);
            if (existing != -1)
                throw std::runtime_error("Duplicate key: " + values[i]);
        }
    }
    int offset = TableFile::insert(path, values, meta);
    for (int i = 0; i < (int) meta.columns.size(); i++) {
        std::string indexKey = getIndexKey(dbName, tableName, meta.columns[i]);
        if (indexes.count(indexKey))
            indexes[indexKey]->insert(values[i], offset);
    }

    return offset;
}

std::vector<std::vector<std::string> > StorageManager::selectRecords(const std::string &dbName,
                                                                     const std::string &tableName,
                                                                     const std::string &whereColumn,
                                                                     const std::string &whereValue,
                                                                     const std::string &op) {
    std::string path = dataPath + dbName + "/" + tableName + ".bin";
    auto meta = SystemCatalog::getInstance().getTableMeta(dbName, tableName);

    // si hay un indice en la columna del where se usa
    if (!whereColumn.empty() && op == "=" && hasIndex(dbName, tableName, whereColumn)) {
        std::string indexKey = getIndexKey(dbName, tableName, whereColumn);
        int offset = indexes[indexKey]->search(whereValue);
        if (offset == -1) return {};
        return {TableFile::readAt(path, offset, meta)};
    }
    // sin indice
    return TableFile::scan(path, meta, whereColumn, whereValue, op);
}

bool StorageManager::updateRecords(const std::string &dbName, const std::string &tableName,
                                   const std::string &setColumn, const std::string &setValue,
                                   const std::string &whereColumn, const std::string &whereValue,
                                   const std::string &op) {
    std::string path = dataPath + dbName + "/" + tableName + ".bin";
    auto meta = SystemCatalog::getInstance().getTableMeta(dbName, tableName);

    // obtener los offset de los registros por actualizar
    std::vector<int> offsets;
    if (!whereColumn.empty() && op == "=" && hasIndex(dbName, tableName, whereColumn)) {
        std::string indexKey = getIndexKey(dbName, tableName, whereColumn);
        int offset = indexes[indexKey]->search(whereValue);
        if (offset != -1) offsets.push_back(offset);
    } else {
        offsets = TableFile::scanOffsets(path, meta, whereColumn, whereValue, op);
    }

    // se actualiza cada registro
    for (int offset: offsets) {
        auto record = TableFile::readAt(path, offset, meta);
        int colIndex = SystemCatalog::getInstance().getColumnIndex(dbName, tableName, setColumn);

        // si la columna tiene un indice, actualizar el indice
        std::string indexKey = getIndexKey(dbName, tableName, setColumn);
        if (indexes.count(indexKey)) {
            indexes[indexKey]->remove(record[colIndex]);
            indexes[indexKey]->insert(setValue, offset);
        }
        record[colIndex] = setValue;
        TableFile::writeAt(path, offset, record, meta);
    }
    return true;
}

bool StorageManager::deleteRecords(const std::string &dbName, const std::string &tableName,
                                   const std::string &whereColumn, const std::string &whereValue,
                                   const std::string &op) {
    std::string path = dataPath + dbName + "/" + tableName + ".bin";
    auto meta = SystemCatalog::getInstance().getTableMeta(dbName, tableName);

    std::vector<int> offsets;
    if (whereColumn.empty() && op == "=" && hasIndex(dbName, tableName, whereColumn)) {
        std::string indexKey = getIndexKey(dbName, tableName, whereColumn);
        int offset = indexes[indexKey]->search(whereValue);
        if (offset != -1) offsets.push_back(offset);
    } else {
        offsets = TableFile::scanOffsets(path, meta, whereColumn, whereValue, op);
    }

    // eliminar los registros e indices
    for (int offset: offsets) {
        auto record = TableFile::readAt(path, offset, meta);
        for (int i = 0; i < (int) meta.columns.size(); i++) {
            std::string indexKey = getIndexKey(dbName, tableName, meta.columns[i]);
            if (indexes.count(indexKey))
                indexes[indexKey]->remove(record[i]);
        }
        TableFile::markDeleted(path, offset, meta);
    }
    return true;
}

// la parte de los indices

bool StorageManager::createIndex(const std::string &dbName, const std::string &tableName, const std::string &columnName,
                                 const std::string &indexName, const std::string &type) {
    std::string indexKey = getIndexKey(dbName, tableName, columnName);
    if (indexes.count(indexKey))
        return false;

    indexes[indexKey] = IndexFactory::create(type);
    SystemCatalog::getInstance().addIndex(dbName, tableName, columnName, indexName, type);

    // cargar los datos que ya se encontraban en el indice
    std::string path = dataPath + dbName + "/" + tableName + ".bin";
    auto meta = SystemCatalog::getInstance().getTableMeta(dbName, tableName);
    int colIndex = SystemCatalog::getInstance().getColumnIndex(dbName, tableName, columnName);

    auto records = TableFile::scanWithOffsets(path, meta);
    for (auto &[record, offset]: records) {
        indexes[indexKey]->insert(record[colIndex], offset);
    }

    return true;
}

void StorageManager::loadIndexes() {
    auto indexList = SystemCatalog::getInstance().getAllIndexes();
    for (auto &idx: indexList) {
        std::string indexKey = getIndexKey(idx.dbName, idx.tableName, idx.columnName);
        indexes[indexKey] = IndexFactory::create(idx.type);

        std::string path = dataPath + idx.dbName + "/" + idx.tableName + ".bin";
        auto meta = SystemCatalog::getInstance().getTableMeta(idx.dbName, idx.tableName);
        int colIndex = SystemCatalog::getInstance().getColumnIndex(idx.dbName, idx.tableName, idx.columnName);

        auto records = TableFile::scanWithOffsets(path, meta);
        for (auto &[record, offset]: records)
            indexes[indexKey]->insert(record[colIndex], offset);
    }
}

// helpers

std::string StorageManager::getIndexKey(const std::string &dbName,
                                        const std::string &tableName,
                                        const std::string &columnName) {
    return dbName + "." + tableName + "." + columnName;
}

bool StorageManager::hasIndex(const std::string &dbName,
                              const std::string &tableName,
                              const std::string &columnName) {
    return indexes.count(getIndexKey(dbName, tableName, columnName)) > 0;
}
