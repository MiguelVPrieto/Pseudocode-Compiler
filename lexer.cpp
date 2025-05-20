#include <vector>
#include <string>
using namespace std;

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
    string value;
};

TokenType lookKeyword(const string& word) {
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

vector<Token> tokenize(const string& code) {
    vector<Token> tokens;
    size_t pos = 0;
    long long line = 1, col = 1;

    while (pos < code.length()) {
        char c = code[pos];

        if (isspace(c)) {
            if (c == '\n') { line++; col = 1; }
            else { col++; }
            pos++;
            continue;
        }

        if (isalpha(c)) {
            size_t start = pos;
            while (pos < code.length() && (isalnum(code[pos]) || code[pos] == '_')) pos++;
            string word = code.substr(start, pos - start);
            TokenType type = lookKeyword(word);
            tokens.push_back({type, word});
            col += word.length();
            continue;
        }

        if (isdigit(c)) {
            size_t start = pos;
            while (pos < code.length() && isdigit(code[pos])) pos++;
            string number = code.substr(start, pos - start);
            tokens.push_back({NUMBER, number});
            col += number.length();
            continue;
        }

        if (c == '"') {
            pos++;
            size_t start = pos;
            while (pos < code.length() && code[pos] != '"') pos++;
            string str = code.substr(start, pos - start);
            if (pos < code.length()) pos++;
            tokens.push_back({STRING, str});
            col += str.length() + 2;
            continue;
        }

        if (code.substr(pos, 1) == "←") {
            tokens.push_back({ASSIGN, "←"});
            pos++;
            col++;
            continue;
        }

        if (pos + 1 < code.length()) {
            string two = code.substr(pos, 2);
            if (two == "<=" || two == ">=" || two == "<>") {
                tokens.push_back({OPERATOR, two});
                pos += 2;
                col += 2;
                continue;
            }
        }

        if (string("+-*/=<>():,").find(c) != string::npos) {
            TokenType type = OPERATOR;
            if (c == '(') type = LPAREN;
            else if (c == ')') type = RPAREN;
            else if (c == ':') type = COLON;
            else if (c == ',') type = COMMA;
            tokens.push_back({type, string(1, c)});
            pos++;
            col++;
            continue;
        }

        tokens.push_back({UNKNOWN, string(1, c)});
        pos++;
        col++;
    }

    tokens.push_back({END, ""});
    return tokens;
}
