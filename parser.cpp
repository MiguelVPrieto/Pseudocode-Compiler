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
    if (token.type == IDENTIFIER) return new VariableExpr(token.value);
    return new LiteralExpr(token.value, token.type);
}

ExprNode* Parser::parsePrimary() {
    if (peek().type == NUMBER || peek().type == STRING || peek().type == IDENTIFIER) return parseLiteral();
    throw std::runtime_error("Expected primary expression");
}

int Parser::getPrecedence(const std::string& op) {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/") return 2;
    return 0;
}

ExprNode* Parser::parseBinaryOpRHS(int exprPrec, ExprNode* lhs) {
    while (true) {
        Token token = peek();
        if (token.type != OPERATOR) break;
        int prec = getPrecedence(token.value);
        if (prec < exprPrec) break;

        advance();
        ExprNode* rhs = parsePrimary();

        Token next = peek();
        if (next.type == OPERATOR) {
            int nextPrec = getPrecedence(next.value);
            if (prec < nextPrec) {
                rhs = parseBinaryOpRHS(prec + 1, rhs);
            }
        }
        lhs = new BinaryExpr(token.value, lhs, rhs);
    }
    return lhs;
}

ASTNode* Parser::parseIf() {
    advance();
    ExprNode* cond = parseExpression();
    expect(THEN, "Expected THEN after IF condition");

    std::vector<ASTNode*> thenBranch;
    while (peek().type != ELSE && peek().type != ENDIF && pos < tokens.size()) {
        thenBranch.push_back(parseStatement());
    }

    std::vector<ASTNode*> elseBranch;
    if (peek().type == ELSE) {
        advance(); // consome ELSE
        while (peek().type != ENDIF && pos < tokens.size()) {
            elseBranch.push_back(parseStatement());
        }
    }
    expect(ENDIF, "Expected ENDIF after IF block");

    return new IfNode(cond, thenBranch, elseBranch);
}

ExprNode* Parser::parseExpression() {
    ExprNode* lhs = parsePrimary();
    return parseBinaryOpRHS(0, lhs);
}

ASTNode* Parser::parseStatement() {
    Token current = peek();

    if (current.type == IF) return parseIf();

    if (current.type == INPUT) {
        advance();
        Token var = expect(IDENTIFIER, "Expected identifier after INPUT");
        return new InputNode(var.value);
    }

    if (current.type == OUTPUT) {
        advance();
        Token next = peek();
        if (next.type == STRING) {
            Token str = advance();
            return new OutputNode(str.value, str.type);
        } else if (next.type == IDENTIFIER) {
            Token str = advance();
            return new OutputNode(str.value, str.type);
        } else {
            throw std::runtime_error("Unexpected token after OUTPUT");
        }
    }

    if (current.type == IDENTIFIER && pos + 1 < tokens.size() && tokens[pos + 1].type == ASSIGN) {
        std::string varName = advance().value;
        advance();
        ExprNode* expr = parseExpression();

        std::string type = "auto";
        if (tokens[pos-1].type == STRING) type = "string";
        else if (tokens[pos-1].type == NUMBER) type = "int";


        return new AssignNode(varName, expr, type);
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
        Token endToken = expect(IDENTIFIER, "Expected end value");
        if (endToken.type != NUMBER && endToken.type != IDENTIFIER) throw std::runtime_error("Expected end value");
        std::string end = endToken.value;

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
