//
// Created by josueaperez on 6/14/26.
//

#ifndef SERVER_SEMANTICVALIDATOR_H
#define SERVER_SEMANTICVALIDATOR_H

#include "Parser.h"
#include <string>

class SemanticValidator {
public:
    void validate(const Statement& stmt, const std::string& currentDb);

private:
    void validateCreateDatabase(const CreateDatabaseStatement& stmt);
    void validateSetDatabase(const SetDatabaseStatement& stmt);
    void validateCreateTable(const CreateTableStatement& stmt,
                             const std::string& currentDb);
    void validateDropTable(const DropTableStatement& stmt,
                           const std::string& currentDb);
    void validateCreateIndex(const CreateIndexStatement& stmt,
                             const std::string& currentDb);
    void validateSelect(const SelectStatement& stmt,
                        const std::string& currentDb);
    void validateInsert(const InsertStatement& stmt,
                        const std::string& currentDb);
    void validateUpdate(const UpdateStatement& stmt,
                        const std::string& currentDb);
    void validateDelete(const DeleteStatement& stmt,
                        const std::string& currentDb);

    // helpers
    void requireDatabase(const std::string& currentDb);
    void requireTableExists(const std::string& dbName,
                            const std::string& tableName);
    void requireColumnExists(const std::string& dbName,
                             const std::string& tableName,
                             const std::string& columnName);
};


#endif //SERVER_SEMANTICVALIDATOR_H
