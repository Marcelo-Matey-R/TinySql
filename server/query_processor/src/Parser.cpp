//
// Created by josueaperez on 6/14/26.
//

#include "Parser.h"
#include <stdexcept>
#include <algorithm>

Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens), pos(0) {
}

// helpers

Token &Parser::current() {
    return tokens[pos];
}

Token &Parser::peek() {
    if (pos + 1 < (int) tokens.size()) return tokens[pos + 1];
    return tokens.back();
}

void Parser::advance() {
    if (pos < (int) tokens.size() - 1) pos++;
}

Token Parser::consume(TokenType type, const std::string &value) {
    Token tok = current();

    if (tok.type != type)
        throw std::runtime_error("Expected token type " +
                                 std::to_string((int) type) + " but got '" + tok.value + "'");

    if (!value.empty() && tok.value != value)
        throw std::runtime_error("Expected '" + value +
                                 "' but got '" + tok.value + "'");

    advance();
    return tok;
}

// la parte importante del parser

Statement Parser::parse() {
    if (current().type == TokenType::EOF_TOKEN)
        throw std::runtime_error("Empty statement");

    std::string keyword = current().value;

    if (keyword == "CREATE") {
        advance();
        if (current().value == "DATABASE") return parseCreateDatabase();
        if (current().value == "TABLE") return parseCreateTable();
        if (current().value == "INDEX") return parseCreateIndex();
        throw std::runtime_error("Expected DATABASE, TABLE or INDEX after CREATE");
    }
    if (keyword == "SET") return parseSetDatabase();
    if (keyword == "DROP") return parseDropTable();
    if (keyword == "SELECT") return parseSelect();
    if (keyword == "INSERT") return parseInsert();
    if (keyword == "UPDATE") return parseUpdate();
    if (keyword == "DELETE") return parseDelete();

    throw std::runtime_error("Unknown statement: " + keyword);
}

// crear base de datos

Statement Parser::parseCreateDatabase() {
    consume(TokenType::KEYWORD, "DATABASE");
    auto name = consume(TokenType::IDENTIFIER);
    return CreateDatabaseStatement{name.value};
}

// setear la base de datos

Statement Parser::parseSetDatabase() {
    consume(TokenType::KEYWORD, "SET");
    consume(TokenType::KEYWORD, "DATABASE");
    auto name = consume(TokenType::IDENTIFIER);
    return SetDatabaseStatement{name.value};
}

// dropear la tabla

Statement Parser::parseDropTable() {
    consume(TokenType::KEYWORD, "DROP");
    consume(TokenType::KEYWORD, "TABLE");
    auto name = consume(TokenType::IDENTIFIER);
    return DropTableStatement{name.value};
}

// crear la tabla

Statement Parser::parseCreateTable() {
    consume(TokenType::KEYWORD, "TABLE");
    auto tableName = consume(TokenType::IDENTIFIER);

    consume(TokenType::LPAREN);

    std::vector<std::string> columns;
    std::vector<std::string> types;
    std::vector<int> sizes;

    while (current().type != TokenType::RPAREN &&
           current().type != TokenType::EOF_TOKEN) {
        auto colName = consume(TokenType::IDENTIFIER);
        int size = 0;
        std::string type = parseType(size);

        columns.push_back(colName.value);
        types.push_back(type);
        sizes.push_back(size);

        if (current().type == TokenType::COMMA) advance();
    }

    consume(TokenType::RPAREN);

    return CreateTableStatement{
        tableName.value, columns, types, sizes
    };
}

std::string Parser::parseType(int &outSize) {
    auto typeTok = consume(TokenType::KEYWORD);
    std::string type = typeTok.value;

    if (type == "INTEGER") {
        outSize = sizeof(int); // 4 bytes
        return "INTEGER";
    }
    if (type == "DOUBLE") {
        outSize = sizeof(double); // 8 bytes
        return "DOUBLE";
    }
    if (type == "DATETIME") {
        outSize = 20; // "YYYY-MM-DD HH:MM:SS\0"
        return "DATETIME";
    }
    if (type == "VARCHAR") {
        consume(TokenType::LPAREN);
        auto sizeTok = consume(TokenType::NUMBER);
        consume(TokenType::RPAREN);
        outSize = std::stoi(sizeTok.value) + 1; // +1 para el \0
        return "VARCHAR";
    }

    throw std::runtime_error("Unknown type: " + type);
}

// crear un index

Statement Parser::parseCreateIndex() {
    consume(TokenType::KEYWORD, "INDEX");
    auto indexName = consume(TokenType::IDENTIFIER);
    consume(TokenType::KEYWORD, "ON");
    auto tableName = consume(TokenType::IDENTIFIER);
    consume(TokenType::LPAREN);
    auto columnName = consume(TokenType::IDENTIFIER);
    consume(TokenType::RPAREN);
    consume(TokenType::KEYWORD, "OF");
    consume(TokenType::KEYWORD, "TYPE");
    auto type = consume(TokenType::KEYWORD);

    if (type.value != "BTREE" && type.value != "BST")
        throw std::runtime_error("Index type must be BTREE or BST");

    return CreateIndexStatement{
        indexName.value, tableName.value, columnName.value, type.value
    };
}

// al hacer un SELECT


Statement Parser::parseSelect() {
    consume(TokenType::KEYWORD, "SELECT");

    std::vector<std::string> columns;

    if (current().type == TokenType::STAR) {
        columns.push_back("*");
        advance();
    } else {
        columns.push_back(consume(TokenType::IDENTIFIER).value);
        while (current().type == TokenType::COMMA) {
            advance();
            columns.push_back(consume(TokenType::IDENTIFIER).value);
        }
    }

    consume(TokenType::KEYWORD, "FROM");
    auto tableName = consume(TokenType::IDENTIFIER);

    std::string whereColumn, whereOp, whereValue;
    std::string orderByColumn, orderByDir;

    if (current().type == TokenType::KEYWORD &&
        current().value == "WHERE") {
        advance();
        whereColumn = consume(TokenType::IDENTIFIER).value;

        if (current().type == TokenType::OPERATOR)
            whereOp = consume(TokenType::OPERATOR).value;
        else if (current().value == "LIKE") {
            whereOp = "like";
            advance();
        } else if (current().value == "NOT") {
            whereOp = "not";
            advance();
        }

        if (current().type == TokenType::STRING)
            whereValue = consume(TokenType::STRING).value;
        else if (current().type == TokenType::NUMBER)
            whereValue = consume(TokenType::NUMBER).value;
        else if (current().type == TokenType::IDENTIFIER)
            whereValue = consume(TokenType::IDENTIFIER).value;
    }

    if (current().type == TokenType::KEYWORD &&
        current().value == "ORDER") {
        advance();
        consume(TokenType::KEYWORD, "BY");
        orderByColumn = consume(TokenType::IDENTIFIER).value;
        orderByDir = current().value == "DESC" ? "DESC" : "ASC";
        advance();
    }

    return SelectStatement{
        columns, tableName.value,
        whereColumn, whereOp, whereValue,
        orderByColumn, orderByDir
    };
}

// al hacer un INSERT

Statement Parser::parseInsert() {
    consume(TokenType::KEYWORD, "INSERT");
    consume(TokenType::KEYWORD, "INTO");
    auto tableName = consume(TokenType::IDENTIFIER);
    consume(TokenType::LPAREN);

    std::vector<std::string> values;
    while (current().type != TokenType::RPAREN &&
           current().type != TokenType::EOF_TOKEN) {
        if (current().type == TokenType::STRING)
            values.push_back(consume(TokenType::STRING).value);
        else if (current().type == TokenType::NUMBER)
            values.push_back(consume(TokenType::NUMBER).value);
        else if (current().type == TokenType::IDENTIFIER)
            values.push_back(consume(TokenType::IDENTIFIER).value);

        if (current().type == TokenType::COMMA) advance();
    }

    consume(TokenType::RPAREN);
    return InsertStatement{tableName.value, values};
}

// al hacer un UPDATE

Statement Parser::parseUpdate() {
    consume(TokenType::KEYWORD, "UPDATE");
    auto tableName = consume(TokenType::IDENTIFIER);
    consume(TokenType::KEYWORD, "SET");
    auto setColumn = consume(TokenType::IDENTIFIER);
    consume(TokenType::OPERATOR, "=");

    std::string setValue;
    if (current().type == TokenType::STRING)
        setValue = consume(TokenType::STRING).value;
    else if (current().type == TokenType::NUMBER)
        setValue = consume(TokenType::NUMBER).value;

    std::string whereColumn, whereOp, whereValue;

    if (current().type == TokenType::KEYWORD &&
        current().value == "WHERE") {
        advance();
        whereColumn = consume(TokenType::IDENTIFIER).value;

        if (current().type == TokenType::OPERATOR)
            whereOp = consume(TokenType::OPERATOR).value;

        if (current().type == TokenType::STRING)
            whereValue = consume(TokenType::STRING).value;
        else if (current().type == TokenType::NUMBER)
            whereValue = consume(TokenType::NUMBER).value;
    }

    return UpdateStatement{
        tableName.value, setColumn.value, setValue,
        whereColumn, whereOp, whereValue
    };
}

// al hacer un DELETE

Statement Parser::parseDelete() {
    consume(TokenType::KEYWORD, "DELETE");
    consume(TokenType::KEYWORD, "FROM");
    auto tableName = consume(TokenType::IDENTIFIER);

    std::string whereColumn, whereOp, whereValue;

    if (current().type == TokenType::KEYWORD &&
        current().value == "WHERE") {
        advance();
        whereColumn = consume(TokenType::IDENTIFIER).value;

        if (current().type == TokenType::OPERATOR)
            whereOp = consume(TokenType::OPERATOR).value;

        if (current().type == TokenType::STRING)
            whereValue = consume(TokenType::STRING).value;
        else if (current().type == TokenType::NUMBER)
            whereValue = consume(TokenType::NUMBER).value;
    }

    return DeleteStatement{
        tableName.value, whereColumn, whereOp, whereValue
    };
}
