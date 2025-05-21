#pragma once
#include <vector>
#include <string>
#include "lexer.hpp"
#include "ast.hpp"

class Parser {
    std::vector<Token> tokens;
    size_t pos;
public:
    Parser(const std::vector<Token>& tokens);
    Token peek() const;
    Token advance();
    bool match(TokenType type);
    Token expect(TokenType type, const std::string& errorMsg);
    ExprNode* parseLiteral();
    ExprNode* parseExpression();
    ASTNode* parseStatement();
    ProgramNode* parseProgram();
};