#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
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

        std::cout << "#include <bits/stdc++.h>\n";
        std::cout << "using namespace std;\n\n";
        std::cout << "int main() {\n";
        ast->generateCode(std::cout);
        std::cout << "}\n";

        fclose(stdout);

        delete ast;

        int result = system("g++ output.cpp -o output.exe");
        if (result != 0) {
            std::cerr << "Error: C++ code compilation failed" << std::endl;
            return 2;
        } else {
            std::cout << "Code compiled succesfully: output.exe created." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
