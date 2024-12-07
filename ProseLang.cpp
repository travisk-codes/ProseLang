#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

// Include the lexer header
#include "ProseLexer.h" // Assume the lexer is implemented in this file
#include "ProseParser.h"
#include "ProseInterpreter.h"

void runRepl() {
    Interpreter interpreter;

    while (true) {
        std::cout << "prose> ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "exit") break;

        try {
            Lexer lexer(input);
            auto tokens = lexer.tokenize();

            Parser parser(tokens);
            auto ast = parser.parseProgram();

            interpreter.interpret(ast);
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }
}

int main() {
    runRepl();
    //Lexer lexer(R"(x is (1 + 2) and "hello")");
    //lexer.tokenize();
    return 0;
}

