#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct ExprNode {
    virtual ~ExprNode() = default;
    virtual string toString() const = 0;
};

struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void generateCode(ostream& out) const = 0;
};

struct ProgramNode : ASTNode {
    vector<ASTNode*> statements;

    ~ProgramNode() {
        for (auto stmt : statements) delete stmt;
    }

    void generateCode(ostream& out) const override {
        for (auto stmt : statements)
            stmt->generateCode(out);
    }
};

struct InputNode : ASTNode {
    string variableName;

    void generateCode(ostream& out) const override {
        out << "cin >> " << variableName << ";" << endl;
    }
};

struct OutputNode : ASTNode {
    string value;

    void generateCode(ostream& out) const override {
        out << "cout << " << value << ";" << endl;
    }
};

struct AssignNode : ASTNode {
    string variableName;
    ExprNode* expr;

    ~AssignNode() { delete expr; }

    void generateCode(ostream& out) const override {
        out << variableName << " = " << expr->toString() << ";" << endl;
    }
};

struct IfNode : ASTNode {
    ExprNode* condition;
    vector<ASTNode*> trueBranch;
    vector<ASTNode*> falseBranch;

    ~IfNode() {
        delete condition;
        for (auto stmt : trueBranch) delete stmt;
        for (auto stmt : falseBranch) delete stmt;
    }

    void generateCode(ostream& out) const override {
        out << "if (" << condition->toString() << ") {" << endl;
        for (auto stmt : trueBranch) stmt->generateCode(out);
        out << "}";
        if (!falseBranch.empty()) {
            out << " else {" << endl;
            for (auto stmt : falseBranch) stmt->generateCode(out);
            out << "}";
        }
        out << endl;
    }
};

struct WhileNode : ASTNode {
    ExprNode* condition;
    vector<ASTNode*> body;

    ~WhileNode() {
        delete condition;
        for (auto stmt : body) delete stmt;
    }

    void generateCode(ostream& out) const override {
        out << "while (" << condition->toString() << ") {" << endl;
        for (auto stmt : body) stmt->generateCode(out);
        out << "}" << endl;
    }
};

struct ForNode : ASTNode {
    string iterator;
    string startExpr;
    string endExpr;
    string stepExpr;
    vector<ASTNode*> body;

    ~ForNode() {
        for (auto stmt : body) delete stmt;
    }

    void generateCode(ostream& out) const override {
        out << "for (int " << iterator << " = " << startExpr
            << "; " << iterator << " <= " << endExpr << "; "
            << iterator << " += " << stepExpr << ") {" << endl;
        for (auto stmt : body) stmt->generateCode(out);
        out << "}" << endl;
    }
};

struct RepeatUntilNode : ASTNode {
    ExprNode* condition;
    vector<ASTNode*> body;

    ~RepeatUntilNode() {
        delete condition;
        for (auto stmt : body) delete stmt;
    }

    void generateCode(ostream& out) const override {
        out << "do {" << endl;
        for (auto stmt : body) stmt->generateCode(out);
        out << "} while (!(" << condition->toString() << "));" << endl;
    }
};

struct LiteralExpr : ExprNode {
    string value;
    string toString() const override { return value; }
};

struct BinaryExpr : ExprNode {
    string op;
    ExprNode* left;
    ExprNode* right;

    ~BinaryExpr() {
        delete left;
        delete right;
    }

    string toString() const override {
        return "(" + left->toString() + " " + op + " " + right->toString() + ")";
    }
};
