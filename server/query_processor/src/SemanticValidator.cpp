//
// Created by josueaperez on 6/14/26.
//

#include "SemanticValidator.h"
#include "../../stored_data_manager/include/SystemCatalog.h"
#include <stdexcept>
#include <algorithm>

//el validador principal

void SemanticValidator::validate(const Statement& stmt,
                                  const std::string& currentDb) {
    if (std::holds_alternative<CreateDatabaseStatement>(stmt))
        validateCreateDatabase(std::get<CreateDatabaseStatement>(stmt));

    else if (std::holds_alternative<SetDatabaseStatement>(stmt))
        validateSetDatabase(std::get<SetDatabaseStatement>(stmt));

    else if (std::holds_alternative<CreateTableStatement>(stmt))
        validateCreateTable(std::get<CreateTableStatement>(stmt), currentDb);

    else if (std::holds_alternative<DropTableStatement>(stmt))
        validateDropTable(std::get<DropTableStatement>(stmt), currentDb);

    else if (std::holds_alternative<CreateIndexStatement>(stmt))
        validateCreateIndex(std::get<CreateIndexStatement>(stmt), currentDb);

    else if (std::holds_alternative<SelectStatement>(stmt))
        validateSelect(std::get<SelectStatement>(stmt), currentDb);

    else if (std::holds_alternative<InsertStatement>(stmt))
        validateInsert(std::get<InsertStatement>(stmt), currentDb);

    else if (std::holds_alternative<UpdateStatement>(stmt))
        validateUpdate(std::get<UpdateStatement>(stmt), currentDb);

    else if (std::holds_alternative<DeleteStatement>(stmt))
        validateDelete(std::get<DeleteStatement>(stmt), currentDb);
}

// validaciones por query


void SemanticValidator::validateCreateDatabase(
    const CreateDatabaseStatement& stmt) {
    if (SystemCatalog::getInstance().databaseExists(stmt.dbName))
        throw std::runtime_error("Database already exists: " + stmt.dbName);
}


void SemanticValidator::validateSetDatabase(
    const SetDatabaseStatement& stmt) {
    if (!SystemCatalog::getInstance().databaseExists(stmt.dbName))
        throw std::runtime_error("Database does not exist: " + stmt.dbName);
}

void SemanticValidator::validateCreateTable(
    const CreateTableStatement& stmt, const std::string& currentDb) {
    requireDatabase(currentDb);

    if (SystemCatalog::getInstance().tableExists(currentDb, stmt.tableName))
        throw std::runtime_error("Table already exists: " + stmt.tableName);

    if (stmt.columns.empty())
        throw std::runtime_error("Table must have at least one column");

    // se verifica que no hayan columnas duplicadas
    for (int i = 0; i < (int)stmt.columns.size(); i++)
        for (int j = i + 1; j < (int)stmt.columns.size(); j++)
            if (stmt.columns[i] == stmt.columns[j])
                throw std::runtime_error("Duplicate column name: " + stmt.columns[i]);
}

void SemanticValidator::validateDropTable(
    const DropTableStatement& stmt, const std::string& currentDb) {
    requireDatabase(currentDb);
    requireTableExists(currentDb, stmt.tableName);

    if (!SystemCatalog::getInstance().tableExists(currentDb, stmt.tableName))
        return; // ya lo valida requiretableExists
}

void SemanticValidator::validateCreateIndex(
    const CreateIndexStatement& stmt, const std::string& currentDb) {
    requireDatabase(currentDb);
    requireTableExists(currentDb, stmt.tableName);
    requireColumnExists(currentDb, stmt.tableName, stmt.columnName);

    if (SystemCatalog::getInstance().indexExists(
            currentDb, stmt.tableName, stmt.columnName))
        throw std::runtime_error(
            "Index already exists on column: " + stmt.columnName);
}

void SemanticValidator::validateSelect(
    const SelectStatement& stmt, const std::string& currentDb) {
    requireDatabase(currentDb);
    requireTableExists(currentDb, stmt.table);

    if (stmt.columns.size() == 1 && stmt.columns[0] == "*") {
        // SELECT * siempre es válido
    } else {
        for (auto& col : stmt.columns)
            requireColumnExists(currentDb, stmt.table, col);
    }

    if (!stmt.whereColumn.empty())
        requireColumnExists(currentDb, stmt.table, stmt.whereColumn);

    if (!stmt.orderByColumn.empty())
        requireColumnExists(currentDb, stmt.table, stmt.orderByColumn);
}

void SemanticValidator::validateInsert(
    const InsertStatement& stmt, const std::string& currentDb) {
    requireDatabase(currentDb);
    requireTableExists(currentDb, stmt.table);

    auto meta = SystemCatalog::getInstance().getTableMeta(currentDb, stmt.table);

    if (stmt.values.size() != meta.columns.size())
        throw std::runtime_error(
            "Expected " + std::to_string(meta.columns.size()) +
            " values but got " + std::to_string(stmt.values.size()));

    // se validan los tipos basicos
    for (int i = 0; i < (int)stmt.values.size(); i++) {
        const std::string& type = meta.types[i];
        const std::string& val = stmt.values[i];

        if (type == "INTEGER") {
            try { std::stoi(val); }
            catch (...) {
                throw std::runtime_error(
                    "Invalid INTEGER value for column " + meta.columns[i] + ": " + val);
            }
        } else if (type == "DOUBLE") {
            try { std::stod(val); }
            catch (...) {
                throw std::runtime_error(
                    "Invalid DOUBLE value for column " + meta.columns[i] + ": " + val);
            }
        } else if (type == "VARCHAR") {
            if ((int)val.size() >= meta.sizes[i])
                throw std::runtime_error(
                    "Value too long for column " + meta.columns[i]);
        }
        // el datetime se procesa en el mismo queryexecutor
    }
}

void SemanticValidator::validateUpdate(
    const UpdateStatement& stmt, const std::string& currentDb) {
    requireDatabase(currentDb);
    requireTableExists(currentDb, stmt.table);
    requireColumnExists(currentDb, stmt.table, stmt.setColumn);

    if (!stmt.whereColumn.empty())
        requireColumnExists(currentDb, stmt.table, stmt.whereColumn);
}

void SemanticValidator::validateDelete(
    const DeleteStatement& stmt, const std::string& currentDb) {
    requireDatabase(currentDb);
    requireTableExists(currentDb, stmt.table);

    if (!stmt.whereColumn.empty())
        requireColumnExists(currentDb, stmt.table, stmt.whereColumn);
}

// helpers

void SemanticValidator::requireDatabase(const std::string& currentDb) {
    if (currentDb.empty())
        throw std::runtime_error(
            "No database selected. Use SET DATABASE first.");
}

void SemanticValidator::requireTableExists(const std::string& dbName,
                                            const std::string& tableName) {
    if (!SystemCatalog::getInstance().tableExists(dbName, tableName))
        throw std::runtime_error("Table does not exist: " + tableName);
}

void SemanticValidator::requireColumnExists(const std::string& dbName,
                                             const std::string& tableName,
                                             const std::string& columnName) {
    if (SystemCatalog::getInstance().getColumnIndex(
            dbName, tableName, columnName) == -1)
        throw std::runtime_error(
            "Column does not exist: " + columnName);
}