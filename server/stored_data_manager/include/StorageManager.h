//
// Created by josueaperez on 6/14/26.
//

#ifndef SERVER_STORAGEMANAGER_H
#define SERVER_STORAGEMANAGER_H

#include "../../data_structures/include/Index.h"
#include "../../data_structures/include/IndexFactory.h"
#include "SystemCatalog.h"
#include "TableFile.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

class StorageManager {
private:
    std::string dataPath;
    std::unordered_map<std::string, std::unique_ptr<Index> > indexes;

    StorageManager();

public:
    static StorageManager &getInstance();

    StorageManager(StorageManager const &) = delete;

    void operator=(StorageManager const &) = delete;

    // parte de bases de datos
    bool createDatabase(const std::string &dbName);

    bool databaseExists(const std::string &dbName);

    // parte de tablas
    bool createTable(const std::string &dbName, const std::string &tableName, const std::vector<std::string> &columns,
                     const std::vector<std::string> &types, const std::vector<int> &sizes);

    bool dropTable(const std::string &dbName, const std::string &tableName);

    bool tableExists(const std::string &dbName, const std::string &tableName);

    bool tableIsEmpty(const std::string &dbName, const std::string &tableName);

    // parte de los registros
    int insertRecord(const std::string &dbName, const std::string &tableName, const std::vector<std::string> &values);

    std::vector<std::vector<std::string> > selectRecords(const std::string &dbName, const std::string &tableName,
                                                         const std::string &whereColumn, const std::string &whereValue,
                                                         const std::string &op);

    bool updateRecords(const std::string &dbName, const std::string &tableName, const std::string &setColumn,
                      const std::string &setValue, const std::string &whereColumn, const std::string &whereValue,
                      const std::string &op);

    bool deleteRecords(const std::string &dbName, const std::string &tableName,
                       const std::string &whereColumn, const std::string &whereValue,
                       const std::string &op);

    // parte de los indices
    bool createIndex(const std::string &dbName, const std::string &tableName,
                     const std::string &columnName, const std::string &indexName,
                     const std::string &type);

    void loadIndexes();

private:
    std::string getIndexKey(const std::string &dbName, const std::string &tableName,
                            const std::string &columnName);

    bool hasIndex(const std::string &dbName, const std::string &tableName,
                  const std::string &columnName);
};


#endif //SERVER_STORAGEMANAGER_H
