//
// Created by josueaperez on 6/14/26.
//

#include "QueryExecutor.h"
#include "../../stored_data_manager/include/StorageManager.h"
#include "../../stored_data_manager/include/SystemCatalog.h"
#include <chrono>
#include <stdexcept>

// la parte del constructor

QueryExecutor::QueryExecutor() : currentDatabase("") {}

// el ejecutor principal

QueryResult QueryExecutor::execute(const std::string& sql) {
    auto start = std::chrono::high_resolution_clock::now();
    QueryResult result;

    try {
        Lexer lexer(sql);
        auto tokens = lexer.tokenize();

        Parser parser(tokens);
        Statement stmt = parser.parse();

        validator.validate(stmt, currentDatabase);

        if (std::holds_alternative<CreateDatabaseStatement>(stmt))
            result = executeCreateDatabase(std::get<CreateDatabaseStatement>(stmt));
        else if (std::holds_alternative<SetDatabaseStatement>(stmt))
            result = executeSetDatabase(std::get<SetDatabaseStatement>(stmt));
        else if (std::holds_alternative<CreateTableStatement>(stmt))
            result = executeCreateTable(std::get<CreateTableStatement>(stmt));
        else if (std::holds_alternative<DropTableStatement>(stmt))
            result = executeDropTable(std::get<DropTableStatement>(stmt));
        else if (std::holds_alternative<CreateIndexStatement>(stmt))
            result = executeCreateIndex(std::get<CreateIndexStatement>(stmt));
        else if (std::holds_alternative<SelectStatement>(stmt))
            result = executeSelect(std::get<SelectStatement>(stmt));
        else if (std::holds_alternative<InsertStatement>(stmt))
            result = executeInsert(std::get<InsertStatement>(stmt));
        else if (std::holds_alternative<UpdateStatement>(stmt))
            result = executeUpdate(std::get<UpdateStatement>(stmt));
        else if (std::holds_alternative<DeleteStatement>(stmt))
            result = executeDelete(std::get<DeleteStatement>(stmt));

    } catch (const std::exception& e) {
        result.success = false;
        result.error = e.what();
    }

    auto end = std::chrono::high_resolution_clock::now();
    result.timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        end - start).count();

    return result;
}

// la parte de crear la base de datos

QueryResult QueryExecutor::executeCreateDatabase(
    const CreateDatabaseStatement& stmt) {
    QueryResult result;
    StorageManager::getInstance().createDatabase(stmt.dbName);
    result.success = true;
    return result;
}

// la parte de setear la base de datos

QueryResult QueryExecutor::executeSetDatabase(
    const SetDatabaseStatement& stmt) {
    QueryResult result;
    currentDatabase = stmt.dbName;
    result.success = true;
    return result;
}

// la parte de crear tablas

QueryResult QueryExecutor::executeCreateTable(
    const CreateTableStatement& stmt) {
    QueryResult result;
    StorageManager::getInstance().createTable(
        currentDatabase, stmt.tableName,
        stmt.columns, stmt.types, stmt.sizes);
    result.success = true;
    return result;
}

// la parte de dropear las tablas

QueryResult QueryExecutor::executeDropTable(
    const DropTableStatement& stmt) {
    QueryResult result;
    bool ok = StorageManager::getInstance().dropTable(
        currentDatabase, stmt.tableName);

    if (!ok) {
        result.success = false;
        result.error = "Cannot drop table: table is not empty";
        return result;
    }
    result.success = true;
    return result;
}

// la parte de crear los indices

QueryResult QueryExecutor::executeCreateIndex(
    const CreateIndexStatement& stmt) {
    QueryResult result;
    bool ok = StorageManager::getInstance().createIndex(
        currentDatabase, stmt.tableName, stmt.columnName,
        stmt.indexName, stmt.type);

    if (!ok) {
        result.success = false;
        result.error = "Cannot create index: duplicate values found in column";
        return result;
    }
    result.success = true;
    return result;
}

// la parte del select

QueryResult QueryExecutor::executeSelect(const SelectStatement& stmt) {
    QueryResult result;

    auto meta = SystemCatalog::getInstance().getTableMeta(
        currentDatabase, stmt.table);

    auto rows = StorageManager::getInstance().selectRecords(
        currentDatabase, stmt.table,
        stmt.whereColumn, stmt.whereValue, stmt.whereOp);

    // se determinan las columnas a mostrar
    std::vector<std::string> columns;
    std::vector<int> colIndexes;

    if (stmt.columns.size() == 1 && stmt.columns[0] == "*") {
        columns = meta.columns;
        for (int i = 0; i < (int)meta.columns.size(); i++)
            colIndexes.push_back(i);
    } else {
        for (auto& col : stmt.columns) {
            columns.push_back(col);
            colIndexes.push_back(
                SystemCatalog::getInstance().getColumnIndex(
                    currentDatabase, stmt.table, col));
        }
    }

    // se dan solo las filas necesarias
    std::vector<std::vector<std::string>> projectedRows;
    for (auto& row : rows) {
        if (row.empty()) continue; // registro eliminado
        std::vector<std::string> projected;
        for (int idx : colIndexes)
            projected.push_back(row[idx]);
        projectedRows.push_back(projected);
    }

    // ORDER BY con quicksort
    if (!stmt.orderByColumn.empty()) {
        int orderColIndex = -1;
        for (int i = 0; i < (int)columns.size(); i++)
            if (columns[i] == stmt.orderByColumn) { orderColIndex = i; break; }

        if (orderColIndex != -1 && !projectedRows.empty()) {
            bool ascending = (stmt.orderByDir != "DESC");
            quicksort(projectedRows, orderColIndex, ascending,
                     0, (int)projectedRows.size() - 1);
        }
    }

    result.columns = columns;
    result.rows = projectedRows;
    result.success = true;
    return result;
}

// la parte de insertar

QueryResult QueryExecutor::executeInsert(const InsertStatement& stmt) {
    QueryResult result;
    StorageManager::getInstance().insertRecord(
        currentDatabase, stmt.table, stmt.values);
    result.success = true;
    return result;
}

// la parte de update

QueryResult QueryExecutor::executeUpdate(const UpdateStatement& stmt) {
    QueryResult result;
    StorageManager::getInstance().updateRecords(
        currentDatabase, stmt.table,
        stmt.setColumn, stmt.setValue,
        stmt.whereColumn, stmt.whereValue, stmt.whereOp);
    result.success = true;
    return result;
}

// la parte de delete
QueryResult QueryExecutor::executeDelete(const DeleteStatement& stmt) {
    QueryResult result;
    StorageManager::getInstance().deleteRecords(
        currentDatabase, stmt.table,
        stmt.whereColumn, stmt.whereValue, stmt.whereOp);
    result.success = true;
    return result;
}

// el quicksort

void QueryExecutor::quicksort(std::vector<std::vector<std::string>>& rows,
                              int colIndex, bool ascending,
                              int low, int high) {
    if (low < high) {
        int pivotIndex = partition(rows, colIndex, ascending, low, high);
        quicksort(rows, colIndex, ascending, low, pivotIndex - 1);
        quicksort(rows, colIndex, ascending, pivotIndex + 1, high);
    }
}

int QueryExecutor::partition(std::vector<std::vector<std::string>>& rows,
                             int colIndex, bool ascending,
                             int low, int high) {
    std::string pivot = rows[high][colIndex];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        bool condition = ascending ?
            (rows[j][colIndex] < pivot) :
            (rows[j][colIndex] > pivot);

        if (condition) {
            i++;
            std::swap(rows[i], rows[j]);
        }
    }
    std::swap(rows[i + 1], rows[high]);
    return i + 1;
}