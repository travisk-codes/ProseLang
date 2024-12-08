#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

// Include the lexer header
#include "ProseLexer.h" // Assume the lexer is implemented in this file
#include "ProseParser.h"
#include "ProseInterpreter.h"

// Function to recursively print the AST
void printAST(const std::shared_ptr<ASTNode>& node, int indent = 0) {
    // Create indentation for each level of the tree
    std::string indentStr(indent, ' ');

    // Print the node's type and value
    std::cout << indentStr << "Node Type: " << static_cast<int>(node->type)
        << ", Value: " << node->value << std::endl;

    // Recursively print the children
    for (const auto& child : node->children) {
        printAST(child, indent + 2);  // Increase indentation for child nodes
    }
}

auto getAST(const std::string& source) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    return parser.parseProgram();
}

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
    //printAST(getAST("print (1 +2)*6"));
    //Lexer lexer(R"(print (1 +2)*6)");
    //lexer.tokenize();
    return 0;
}

