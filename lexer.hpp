#pragma once
#include <string>
#include <vector>

enum TokenType {
    INPUT, OUTPUT, IDENTIFIER, NUMBER, STRING, OPERATOR,
    TRUE, FALSE, ASSIGN,
    IF, THEN, ELSE, ENDIF,
    FOR, TO, NEXT,
    WHILE, ENDWHILE, REPEAT, UNTIL,
    PROCEDURE, FUNCTION, RETURN,
    COLON, LPAREN, RPAREN, COMMA,
    UNKNOWN, END
};

struct Token {
    TokenType type;
    std::string value;
};

TokenType lookKeyword(const std::string& word);
std::vector<Token> tokenize(const std::string& code);
