//
// Created by josueaperez on 6/14/26.
//

#ifndef SERVER_QUERYEXECUTOR_H
#define SERVER_QUERYEXECUTOR_H

#include "Lexer.h"
#include "Parser.h"
#include "SemanticValidator.h"
#include <string>
#include <vector>

struct QueryResult {
    std::vector<std::string> columns;
    std::vector<std::vector<std::string>> rows;
    std::string error;
    bool success;
    long long timeMs;
};

class QueryExecutor {
private:
    std::string currentDatabase;
    SemanticValidator validator;

public:
    QueryExecutor();
    QueryResult execute(const std::string& sql);

private:
    QueryResult executeCreateDatabase(const CreateDatabaseStatement& stmt);
    QueryResult executeSetDatabase(const SetDatabaseStatement& stmt);
    QueryResult executeCreateTable(const CreateTableStatement& stmt);
    QueryResult executeDropTable(const DropTableStatement& stmt);
    QueryResult executeCreateIndex(const CreateIndexStatement& stmt);
    QueryResult executeSelect(const SelectStatement& stmt);
    QueryResult executeInsert(const InsertStatement& stmt);
    QueryResult executeUpdate(const UpdateStatement& stmt);
    QueryResult executeDelete(const DeleteStatement& stmt);

    // quicksort para ORDER BY
    void quicksort(std::vector<std::vector<std::string>>& rows,
                   int colIndex, bool ascending, int low, int high);
    int partition(std::vector<std::vector<std::string>>& rows,
                  int colIndex, bool ascending, int low, int high);
};


#endif //SERVER_QUERYEXECUTOR_H
