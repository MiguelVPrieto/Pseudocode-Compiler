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
    ExprNode* parsePrimary();
    ASTNode* parseIf();
    int getPrecedence(const std::string& op);
    ExprNode* parseBinaryOpRHS(int exprPrec, ExprNode* lhs);
    ASTNode* parseStatement();
    ProgramNode* parseProgram();
};