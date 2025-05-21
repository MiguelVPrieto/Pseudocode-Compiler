#include "ast.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include <stdexcept>


Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

Token Parser::peek() const { return tokens[pos]; }
Token Parser::advance() { return tokens[pos++]; }

bool Parser::match(TokenType type) {
    if (pos < tokens.size() && tokens[pos].type == type) {
        pos++;
        return true;
    }
    return false;
}

Token Parser::expect(TokenType type, const std::string& errorMsg) {
    if (pos < tokens.size() && tokens[pos].type == type)
        return tokens[pos++];
    throw std::runtime_error("Parser error: " + errorMsg);
}

ExprNode* Parser::parseLiteral() {
    Token token = advance();
    return new LiteralExpr(token.value);
}

ExprNode* Parser::parseExpression() {
    if (peek().type == NUMBER || peek().type == STRING || peek().type == IDENTIFIER)
        return parseLiteral();
    throw std::runtime_error("Expected expression");
}

ASTNode* Parser::parseStatement() {
    Token current = peek();

    if (current.type == INPUT) {
        advance();
        Token var = expect(IDENTIFIER, "Expected identifier after INPUT");
        return new InputNode(var.value);
    }

    if (current.type == OUTPUT) {
        advance();
        Token value = expect(IDENTIFIER, "Expected identifier after OUTPUT");
        return new OutputNode(value.value);
    }

    if (current.type == IDENTIFIER && pos + 1 < tokens.size() && tokens[pos + 1].type == ASSIGN) {
        std::string varName = advance().value;
        advance();
        ExprNode* expr = parseExpression();
        return new AssignNode(varName, expr);
    }

    if (current.type == IF) {
        advance();
        ExprNode* condition = parseExpression();
        expect(THEN, "Expected THEN after IF condition");

        std::vector<ASTNode*> trueBranch;
        std::vector<ASTNode*> falseBranch;

        while (peek().type != ELSE && peek().type != ENDIF) {
            trueBranch.push_back(parseStatement());
        }

        if (match(ELSE)) {
            while (peek().type != ENDIF) {
                falseBranch.push_back(parseStatement());
            }
        }

        expect(ENDIF, "Expected ENDIF after IF block");
        return new IfNode(condition, trueBranch, falseBranch);
    }

    if (current.type == WHILE) {
        advance();
        ExprNode* condition = parseExpression();

        std::vector<ASTNode*> body;
        while (peek().type != ENDWHILE) {
            body.push_back(parseStatement());
        }
        expect(ENDWHILE, "Expected ENDWHILE after WHILE loop");
        return new WhileNode(condition, body);
    }

    if (current.type == REPEAT) {
        advance();
        std::vector<ASTNode*> body;
        while (peek().type != UNTIL) {
            body.push_back(parseStatement());
        }
        advance();
        ExprNode* condition = parseExpression();
        return new RepeatUntilNode(condition, body);
    }

    if (current.type == FOR) {
        advance();
        std::string iterator = expect(IDENTIFIER, "Expected loop variable after FOR").value;
        expect(ASSIGN, "Expected ← after loop variable");
        std::string start = expect(NUMBER, "Expected start value").value;
        expect(TO, "Expected TO after start value");
        std::string end = expect(NUMBER, "Expected end value").value;

        std::string step = "1";
        std::vector<ASTNode*> body;
        while (peek().type != NEXT) {
            body.push_back(parseStatement());
        }
        expect(NEXT, "Expected NEXT to close FOR loop");

        return new ForNode(iterator, start, end, step, body);
    }

    throw std::runtime_error("Unknown statement starting with: " + current.value);
}

ProgramNode* Parser::parseProgram() {
    auto* program = new ProgramNode();

    while (peek().type != END) {
        program->statements.push_back(parseStatement());
    }

    return program;
}
