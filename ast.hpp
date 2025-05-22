#pragma once
#include "lexer.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>

struct ExprNode {
    virtual ~ExprNode() = default;
    virtual std::string toString() const = 0;
};

struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void generateCode(std::ostream& out) const = 0;
};

struct ProgramNode : ASTNode {
    std::vector<ASTNode*> statements;
    std::map<std::string, std::string> varTypes;

    ~ProgramNode() {
        for (auto stmt : statements) delete stmt;
    }

    void generateCode(std::ostream& out) const override {
        for (auto stmt : statements)
            stmt->generateCode(out);
    }
};

struct InputNode : ASTNode {
    std::string variableName;
    InputNode(const std::string& var) : variableName(var) {}

    void generateCode(std::ostream& out) const override {
        out << "\tcin >> " << variableName << ";" << std::endl;
    }
};

struct OutputNode : ASTNode {
    std::string value;
    TokenType type;
    OutputNode(const std::string& val, TokenType t) : value(val), type(t) {}

    void generateCode(std::ostream& out) const override {
        if (type == STRING) {
            out << "\tcout << " << "\"" << value << "\"" << " << endl;" << std::endl;
        } else {
            out << "\tcout << " << value << " << endl;" << std::endl;
        }
    }
};

struct AssignNode : ASTNode {
    std::string variableName;
    ExprNode* expr;
    std::string type;
    AssignNode(const std::string& var, ExprNode* e, const std::string& t) : variableName(var), expr(e), type(t) {}

    ~AssignNode() { delete expr; }

    void generateCode(std::ostream& out) const override {
        out << "\t" << type << " " << variableName << " = " << expr->toString() << ";" << std::endl;
    }
};

struct IfNode : ASTNode {
    ExprNode* condition;
    std::vector<ASTNode*> trueBranch;
    std::vector<ASTNode*> falseBranch;
    IfNode(ExprNode* cond, const std::vector<ASTNode*>& t, const std::vector<ASTNode*>& f)
            : condition(cond), trueBranch(t), falseBranch(f) {}

    ~IfNode() {
        delete condition;
        for (auto stmt : trueBranch) delete stmt;
        for (auto stmt : falseBranch) delete stmt;
    }

    void generateCode(std::ostream& out) const override {
        out << "\tif (" << condition->toString() << ") {" << std::endl;
        for (auto stmt : trueBranch) stmt->generateCode(out);
        out << "\t}";
        if (!falseBranch.empty()) {
            out << " else {" << std::endl;
            for (auto stmt : falseBranch) stmt->generateCode(out);
            out << "\t}";
        }
        out << std::endl;
    }
};

struct WhileNode : ASTNode {
    ExprNode* condition;
    std::vector<ASTNode*> body;
    WhileNode(ExprNode* cond, const std::vector<ASTNode*>& b) : condition(cond), body(b) {}

    ~WhileNode() {
        delete condition;
        for (auto stmt : body) delete stmt;
    }

    void generateCode(std::ostream& out) const override {
        out << "\twhile (" << condition->toString() << ") {" << std::endl;
        for (auto stmt : body) stmt->generateCode(out);
        out << "\t}" << std::endl;
    }
};

struct ForNode : ASTNode {
    std::string iterator;
    std::string startExpr;
    std::string endExpr;
    std::string stepExpr;
    std::vector<ASTNode*> body;
    ForNode(const std::string& it, const std::string& start, const std::string& end, const std::string& step, const std::vector<ASTNode*>& b)
            : iterator(it), startExpr(start), endExpr(end), stepExpr(step), body(b) {}

    ~ForNode() {
        for (auto stmt : body) delete stmt;
    }

    void generateCode(std::ostream& out) const override {
        out << "\tfor (int " << iterator << " = " << startExpr
            << "; " << iterator << " <= " << endExpr << "; "
            << iterator << " += " << stepExpr << ") {" << std::endl;
        for (auto stmt : body) stmt->generateCode(out);
        out << "\t}" << std::endl;
    }
};

struct RepeatUntilNode : ASTNode {
    ExprNode* condition;
    std::vector<ASTNode*> body;
    RepeatUntilNode(ExprNode* cond, const std::vector<ASTNode*>& b) : condition(cond), body(b) {}

    ~RepeatUntilNode() {
        delete condition;
        for (auto stmt : body) delete stmt;
    }

    void generateCode(std::ostream& out) const override {
        out << "\tdo {" << std::endl;
        for (auto stmt : body) stmt->generateCode(out);
        out << "} while (!(" << condition->toString() << "));" << std::endl;
    }
};

struct LiteralExpr : ExprNode {
    std::string value;
    TokenType type;
    LiteralExpr(const std::string& val, TokenType t) : value(val), type(t) {}
    std::string toString() const override {
        if (type == STRING) return "\"" + value + "\"";
        return value;
    }
};

struct BinaryExpr : ExprNode {
    std::string op;
    ExprNode* left;
    ExprNode* right;
    BinaryExpr(const std::string& oper, ExprNode* l, ExprNode* r) : op(oper), left(l), right(r) {}

    ~BinaryExpr() {
        delete left;
        delete right;
    }

    std::string toString() const override {
        return "(" + left->toString() + " " + op + " " + right->toString() + ")";
    }
};
