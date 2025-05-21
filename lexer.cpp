#include "lexer.hpp"
#include <cctype>

TokenType lookKeyword(const std::string& word) {
    if (word == "INPUT") return INPUT;
    if (word == "OUTPUT") return OUTPUT;
    if (word == "TRUE") return TRUE;
    if (word == "FALSE") return FALSE;
    if (word == "IF") return IF;
    if (word == "THEN") return THEN;
    if (word == "ELSE") return ELSE;
    if (word == "ENDIF") return ENDIF;
    if (word == "FOR") return FOR;
    if (word == "TO") return TO;
    if (word == "NEXT") return NEXT;
    if (word == "WHILE") return WHILE;
    if (word == "ENDWHILE") return ENDWHILE;
    if (word == "REPEAT") return REPEAT;
    if (word == "UNTIL") return UNTIL;
    if (word == "PROCEDURE") return PROCEDURE;
    if (word == "FUNCTION") return FUNCTION;
    if (word == "RETURN") return RETURN;
    return IDENTIFIER;
}

std::vector<Token> tokenize(const std::string& code) {
    std::vector<Token> tokens;
    size_t pos = 0;
    while (pos < code.length()) {
        char c = code[pos];

        if (isspace(c)) {
            pos++;
            continue;
        }

        if (isalpha(c)) {
            size_t start = pos;
            while (pos < code.length() && (isalnum(code[pos]) || code[pos] == '_')) pos++;
            std::string word = code.substr(start, pos - start);
            TokenType type = lookKeyword(word);
            tokens.push_back({type, word});
            continue;
        }

        if (isdigit(c)) {
            size_t start = pos;
            while (pos < code.length() && isdigit(code[pos])) pos++;
            std::string number = code.substr(start, pos - start);
            tokens.push_back({NUMBER, number});
            continue;
        }

        if (c == '"') {
            pos++;
            size_t start = pos;
            while (pos < code.length() && code[pos] != '"') pos++;
            std::string str = code.substr(start, pos - start);
            if (pos < code.length()) pos++;
            tokens.push_back({STRING, str});
            continue;
        }

        if (pos + 1 < code.length() && code.substr(pos, 2) == "<-") {
            tokens.push_back({ASSIGN, "<-"});
            pos += 2;
            continue;
        }

        if (pos + 1 < code.length()) {
            std::string two = code.substr(pos, 2);
            if (two == "<=" || two == ">=" || two == "<>") {
                tokens.push_back({OPERATOR, two});
                pos += 2;
                continue;
            }
        }

        if (std::string("+-*/=<>():,").find(c) != std::string::npos) {
            TokenType type = OPERATOR;
            if (c == '(') type = LPAREN;
            else if (c == ')') type = RPAREN;
            else if (c == ':') type = COLON;
            else if (c == ',') type = COMMA;
            tokens.push_back({type, std::string(1, c)});
            pos++;
            continue;
        }

        tokens.push_back({UNKNOWN, std::string(1, c)});
        pos++;
    }

    tokens.push_back({END, ""});
    return tokens;
}
