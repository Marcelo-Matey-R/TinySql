//
// Created by josueaperez on 6/14/26.
//

#ifndef SERVER_PARSER_H
#define SERVER_PARSER_H

#include "Lexer.h"
#include <string>
#include <vector>
#include <variant>

// estructura de los querys

struct CreateDatabaseStatement {
    std::string dbName;
};

struct SetDatabaseStatement {
    std::string dbName;
};

struct CreateTableStatement {
    std::string tableName;
    std::vector<std::string> columns;
    std::vector<std::string> types;
    std::vector<int> sizes;
};

struct DropTableStatement {
    std::string tableName;
};

struct CreateIndexStatement {
    std::string indexName;
    std::string tableName;
    std::string columnName;
    std::string type;
};

struct SelectStatement {
    std::vector<std::string> columns;
    std::string table;
    std::string whereColumn;
    std::string whereOp;
    std::string whereValue;
    std::string orderByColumn;
    std::string orderByDir;
};

struct InsertStatement {
    std::string table;
    std::vector<std::string> values;
};

struct UpdateStatement {
    std::string table;
    std::string setColumn;
    std::string setValue;
    std::string whereColumn;
    std::string whereOp;
    std::string whereValue;
};

struct DeleteStatement {
    std::string table;
    std::string whereColumn;
    std::string whereOp;
    std::string whereValue;
};

// un variant puede ser cualquier tipo de query
using Statement = std::variant<
    CreateDatabaseStatement,
    SetDatabaseStatement,
    CreateTableStatement,
    DropTableStatement,
    CreateIndexStatement,
    SelectStatement,
    InsertStatement,
    UpdateStatement,
    DeleteStatement
>;

class Parser {
private:
    std::vector<Token> tokens;
    int pos;

public:
    explicit Parser(const std::vector<Token> &tokens);

    Statement parse();

private:
    Token &current();

    Token &peek();

    void advance();

    Token consume(TokenType type, const std::string &value = "");

    Statement parseCreateDatabase();

    Statement parseSetDatabase();

    Statement parseCreateTable();

    Statement parseDropTable();

    Statement parseCreateIndex();

    Statement parseSelect();

    Statement parseInsert();

    Statement parseUpdate();

    Statement parseDelete();

    std::string parseType(int &outSize);
};


#endif //SERVER_PARSER_H
