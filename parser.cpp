#include <vector>
#include <string>
#include <stdexcept>
#include "lexer.cpp"
#include "ast.hpp"
using namespace std;

class Parser {
    const vector<Token>& tokens;
    size_t pos = 0;

public:
    Parser(const vector<Token>& tokens) : tokens(tokens) {}

    Token peek() const { return tokens[pos]; }
    Token advance() { return tokens[pos++]; }

    bool match(TokenType type) {
        if (pos < tokens.size() && tokens[pos].type == type) {
            pos++;
            return true;
        }
        return false;
    }

    Token expect(TokenType type, const string& errorMsg) {
        if (pos < tokens.size() && tokens[pos].type == type)
            return tokens[pos++];
        throw runtime_error("Parser error: " + errorMsg);
    }

    ExprNode* parseLiteral() {
        Token token = advance();
        return new LiteralExpr{token.value};
    }

    ExprNode* parseExpression() {
        if (peek().type == NUMBER || peek().type == STRING || peek().type == IDENTIFIER)
            return parseLiteral();
        throw runtime_error("Expected expression");
    }

    ASTNode* parseStatement() {
        Token current = peek();

        if (current.type == INPUT) {
            advance();
            Token var = expect(IDENTIFIER, "Expected identifier after INPUT");
            return new InputNode{var.value};
        }

        if (current.type == OUTPUT) {
            advance();
            Token value = expect(IDENTIFIER, "Expected identifier after OUTPUT");
            return new OutputNode{value.value};
        }

        if (current.type == IDENTIFIER && tokens[pos + 1].type == ASSIGN) {
            string varName = advance().value;
            advance();
            ExprNode* expr = parseExpression();
            return new AssignNode{varName, expr};
        }

        if (current.type == IF) {
            advance();
            ExprNode* condition = parseExpression();
            expect(THEN, "Expected THEN after IF condition");

            vector<ASTNode*> trueBranch;
            vector<ASTNode*> falseBranch;

            while (peek().type != ELSE && peek().type != ENDIF) {
                trueBranch.push_back(parseStatement());
            }

            if (match(ELSE)) {
                while (peek().type != ENDIF) {
                    falseBranch.push_back(parseStatement());
                }
            }

            expect(ENDIF, "Expected ENDIF after IF block");
            return new IfNode{condition, trueBranch, falseBranch};
        }

        if (current.type == WHILE) {
            advance();
            ExprNode* condition = parseExpression();

            vector<ASTNode*> body;
            while (peek().type != ENDWHILE) {
                body.push_back(parseStatement());
            }
            expect(ENDWHILE, "Expected ENDWHILE after WHILE loop");
            return new WhileNode{condition, body};
        }

        if (current.type == REPEAT) {
            advance();
            vector<ASTNode*> body;
            while (peek().type != UNTIL) {
                body.push_back(parseStatement());
            }
            advance();
            ExprNode* condition = parseExpression();
            return new RepeatUntilNode{condition, body};
        }

        if (current.type == FOR) {
            advance();
            string iterator = expect(IDENTIFIER, "Expected loop variable after FOR").value;
            expect(ASSIGN, "Expected ← after loop variable");
            string start = expect(NUMBER, "Expected start value").value;
            expect(TO, "Expected TO after start value");
            string end = expect(NUMBER, "Expected end value").value;

            string step = "1";
            vector<ASTNode*> body;
            while (peek().type != NEXT) {
                body.push_back(parseStatement());
            }
            expect(NEXT, "Expected NEXT to close FOR loop");

            return new ForNode{iterator, start, end, step, body};
        }

        throw runtime_error("Unknown statement starting with: " + current.value);
    }

    ProgramNode* parseProgram() {
        auto* program = new ProgramNode();

        while (peek().type != END) {
            program->statements.push_back(parseStatement());
        }

        return program;
    }
};
