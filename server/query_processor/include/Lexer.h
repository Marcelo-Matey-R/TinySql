//
// Created by josueaperez on 6/14/26.
//

#ifndef SERVER_LEXER_H
#define SERVER_LEXER_H

#include <string>
#include <vector>
#include <unordered_set>

enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    STRING,
    NUMBER,
    OPERATOR,
    LPAREN,
    RPAREN,
    COMMA,
    SEMICOLON,
    STAR,
    EOF_TOKEN
};

struct Token {
    TokenType type;
    std::string value;

    Token(TokenType type, const std::string &value)
        : type(type), value(value) {
    }
};

class Lexer {
private:
    std::string input;
    int pos;
    static const std::unordered_set<std::string> KEYWORDS;

public:
    explicit Lexer(const std::string &input);

    std::vector<Token> tokenize();

private:
    char current();

    char peek();

    void advance();

    void skipWhitespace();

    void skipComment();

    Token readKeywordOrIdentifier();

    Token readString();

    Token readNumber();

    Token readOperator();
};


#endif //SERVER_LEXER_H
