#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "lexer.cpp"
#include "parser.cpp"
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        cerr << "Error: Could not open file " << argv[1] << endl;
        return 1;
    }

    stringstream buffer;
    buffer << inputFile.rdbuf();
    string code = buffer.str();
    inputFile.close();

    vector<Token> tokens = tokenize(code);
    Parser parser(tokens);
    ProgramNode* ast = parser.parseProgram();

    freopen("output.cpp", "w", stdout);

    cout << "#include <iostream>" << endl;
    cout << "using namespace std;" << endl << endl;
    cout << "int main() {" << endl;
    ast->generateCode(cout);
    cout << "    return 0;" << endl;
    cout << "}" << endl;

    delete ast;
}
