//
// Created by josueaperez on 6/14/26.
//

#include "Lexer.h"
#include <stdexcept>
#include <cctype>
#include <algorithm>

// palabras clave

const std::unordered_set<std::string> Lexer::KEYWORDS = {
    "SELECT", "FROM", "WHERE", "ORDER", "BY", "ASC", "DESC",
    "INSERT", "INTO", "VALUES",
    "UPDATE", "SET",
    "DELETE",
    "CREATE", "DATABASE", "TABLE", "INDEX", "ON", "OF", "TYPE",
    "DROP",
    "BTREE", "BST",
    "INTEGER", "DOUBLE", "VARCHAR", "DATETIME",
    "LIKE", "NOT", "AND", "OR",
    "NULL"
};

// constructor

Lexer::Lexer(const std::string &input) : input(input), pos(0) {
}

// la parte de tokens

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (pos < (int) input.size()) {
        skipWhitespace();

        if (pos >= (int) input.size()) break;

        // Comentario
        if (current() == '/' && peek() == '/') {
            skipComment();
            continue;
        }

        if (current() == '(') {
            tokens.push_back({TokenType::LPAREN, "("});
            advance();
        } else if (current() == ')') {
            tokens.push_back({TokenType::RPAREN, ")"});
            advance();
        } else if (current() == ',') {
            tokens.push_back({TokenType::COMMA, ","});
            advance();
        } else if (current() == ';') {
            tokens.push_back({TokenType::SEMICOLON, ";"});
            advance();
        } else if (current() == '*') {
            tokens.push_back({TokenType::STAR, "*"});
            advance();
        } else if (current() == '"') {
            tokens.push_back(readString());
        } else if (std::isdigit(current()) || current() == '-') {
            tokens.push_back(readNumber());
        } else if (current() == '>' || current() == '<' ||
                   current() == '=' || current() == '!') {
            tokens.push_back(readOperator());
        } else if (std::isalpha(current()) || current() == '_') {
            tokens.push_back(readKeywordOrIdentifier());
        } else {
            throw std::runtime_error(
                std::string("Unexpected character: ") + current());
        }
    }

    tokens.push_back({TokenType::EOF_TOKEN, ""});
    return tokens;
}

// helpers

char Lexer::current() {
    return input[pos];
}

char Lexer::peek() {
    if (pos + 1 < (int) input.size()) return input[pos + 1];
    return '\0';
}

void Lexer::advance() {
    pos++;
}

void Lexer::skipWhitespace() {
    while (pos < (int) input.size() &&
           std::isspace(current()))
        advance();
}

void Lexer::skipComment() {
    while (pos < (int) input.size() && current() != '\n')
        advance();
}

//lectores

Token Lexer::readKeywordOrIdentifier() {
    std::string value;
    while (pos < (int) input.size() &&
           (std::isalnum(current()) || current() == '_')) {
        value += current();
        advance();
    }

    // se convierte a mayúsculas para comparar con keywords
    std::string upper = value;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

    if (KEYWORDS.count(upper))
        return {TokenType::KEYWORD, upper};

    return {TokenType::IDENTIFIER, value};
}

Token Lexer::readString() {
    advance(); // salta la comilla inicial
    std::string value;
    while (pos < (int) input.size() && current() != '"') {
        value += current();
        advance();
    }
    advance(); // salta la comilla final
    return {TokenType::STRING, value};
}

Token Lexer::readNumber() {
    std::string value;
    if (current() == '-') {
        value += current();
        advance();
    }
    while (pos < (int) input.size() &&
           (std::isdigit(current()) || current() == '.')) {
        value += current();
        advance();
    }
    return {TokenType::NUMBER, value};
}

Token Lexer::readOperator() {
    std::string value;
    value += current();
    advance();

    // manejar == que el spec usa en algunos ejemplos
    if (current() == '=') {
        value += current();
        advance();
    }

    // normalizar == a =
    if (value == "==") value = "=";

    return {TokenType::OPERATOR, value};
}
