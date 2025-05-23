#pragma once
#include "lexer.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>

inline std::set<std::string> variables;
inline std::set<std::string> stringVars;

struct ExprNode {
    virtual ~ExprNode() = default;
    virtual std::string toString() const = 0;
    virtual void generateCode(std::ostream& out) const = 0;
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
        if (variables.find(variableName) == variables.end()) {
            out << "\tstring " << variableName << ";" << std::endl;
            variables.insert(variableName);
            stringVars.insert(variableName);
        }
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
        if (variables.find(variableName) == variables.end()) {
            out << "\t" << type << " " << variableName << " = ";
            if (type == "STRING") {
                stringVars.insert(variableName);
            }
            variables.insert(variableName);
        } else {
            out << "\t" << variableName << " = ";
        }
        expr->generateCode(out);
        out << ";" << std::endl;
    }
};

struct IfNode : public ASTNode {
    ExprNode* condition;
    std::vector<ASTNode*> thenBranch;
    std::vector<ASTNode*> elseBranch;
    IfNode(ExprNode* cond, const std::vector<ASTNode*>& thenB, const std::vector<ASTNode*>& elseB)
            : condition(cond), thenBranch(std::move(thenB)), elseBranch(std::move(elseB)) {}

    ~IfNode() {
        delete condition;
        for (auto stmt : thenBranch) delete stmt;
        for (auto stmt : elseBranch) delete stmt;
    }

    void generateCode(std::ostream& out) const override {
        out << "\tif (";
        condition->generateCode(out);
        out << ") {\n";
        for (auto stmt : thenBranch) stmt->generateCode(out);
        out << "\t}";
        if (!elseBranch.empty()) {
            out << " else {\n";
            for (auto stmt : elseBranch) stmt->generateCode(out);
            out << "\t}";
        }
        out << "\n";
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
            << "; " << iterator << " <= ";
        if (stringVars.find(endExpr) != stringVars.end()) {
            out << "stoi(" << endExpr << ")";
        } else {
            out << endExpr;
        }
        out << "; " << iterator << " += " << stepExpr << ") {" << std::endl;
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
    void generateCode(std::ostream& out) const override {
        if (type == STRING) out << "\"" << value << "\"";
        else out << value;
    }
};

struct VariableExpr : ExprNode {
    std::string name;
    VariableExpr(const std::string& n) : name(n) {}
    std::string toString() const override { return name; }
    void generateCode(std::ostream& out) const override { out << name; }
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
    void generateCode(std::ostream& out) const override {
        out << "(";
        auto genOperand = [&](ExprNode* expr) {
            auto varExpr = dynamic_cast<VariableExpr*>(expr);
            if (varExpr && stringVars.find(varExpr->name) != stringVars.end()) {
                out << "stoi(";
                expr->generateCode(out);
                out << ")";
            } else {
                expr->generateCode(out);
            }
        };
        genOperand(left);
        if (op == "=") out << " == ";
        else out << " " << op << " ";
        genOperand(right);
        out << ")";
    }
};