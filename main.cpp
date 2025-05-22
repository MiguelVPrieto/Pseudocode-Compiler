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

        std::cout << "#include <iostream>\n";
        std::cout << "using namespace std;\n\n";
        std::cout << "int main() {\n";
        ast->generateCode(std::cout);
        std::cout << "}\n";

        delete ast;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
