//
// Created by josueaperez on 6/14/26.
//

#ifndef SERVER_SYSTEMCATALOG_H
#define SERVER_SYSTEMCATALOG_H

#include <string>
#include <vector>

struct TableMeta {
    std::vector<std::string> columns;
    std::vector<std::string> types;
    std::vector<int> sizes;
    int recordSize;
};

struct IndexInfo {
    std::string dbName;
    std::string tableName;
    std::string columnName;
    std::string indexName;
    std::string type;
};

class SystemCatalog {
private:
    std::string catalogPath;

    SystemCatalog();
public:
    static SystemCatalog& getInstance();
    SystemCatalog(const SystemCatalog&) = delete;
    void operator=(const SystemCatalog&) = delete;

    // la parte de la base de datos
    void addDatabase(const std::string& dbName);
    bool databaseExists(const std::string& dbName);
    std::vector<std::string> getAllDatabases();

    //la parte de las tablas
    void addTable(const std::string& dbName,
                  const std::string& tableName,
                  const std::vector<std::string>& columns,
                  const std::vector<std::string>& types,
                  const std::vector<int>& sizes);
    void removeTable(const std::string& dbName, const std::string& tableName);
    bool tableExists(const std::string& dbName, const std::string& tableName);
    std::vector<std::string> getTablesForDatabase(const std::string& dbName);

    // la parte de columnas
    TableMeta getTableMeta(const std::string& dbName, const std::string& tableName);
    int getColumnIndex(const std::string& dbName,
                       const std::string& tableName,
                       const std::string& columnName);

    //la parte de indices
    void addIndex(const std::string& dbName,
                  const std::string& tableName,
                  const std::string& columnName,
                  const std::string& indexName,
                  const std::string& type);
    std::vector<IndexInfo> getAllIndexes();
    bool indexExists(const std::string& dbName,
                     const std::string& tableName,
                     const std::string& columnName);

private:
    void writeString(std::ofstream& file, const std::string& str, int size);
    std::string readString(std::ifstream& file, int size);
};


#endif //SERVER_SYSTEMCATALOG_H
