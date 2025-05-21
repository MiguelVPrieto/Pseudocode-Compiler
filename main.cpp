#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.hpp"
#include "parser.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    std::string code = buffer.str();
    inputFile.close();

    std::vector<Token> tokens = tokenize(code);
    Parser parser(tokens);
    try {
        ProgramNode* ast = parser.parseProgram();
        freopen("output.cpp", "w", stdout);

        std::cout << "#include <bits/stdc++.h>" << std::endl;
        std::cout << "using namespace std;" << std::endl << std::endl;
        std::cout << "#define endl '\\n'" << std::endl << std::endl;
        std::cout << "int main() {" << std::endl;
        std::cout << "\tios_base::sync_with_stdio(false);" << std::endl << std::endl;
        std::cout << "\tcin.tie(nullptr);" << std::endl << std::endl;
        ast->generateCode(std::cout);
        std::cout << "}" << std::endl;

        delete ast;
    } catch (const std::exception& e) {
        std::cerr << "Erro de parsing: " << e.what() << std::endl;
        return 1;
    }
}
