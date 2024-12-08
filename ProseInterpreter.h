#ifndef PROSEINTERPRETER_H
#define PROSEINTERPRETER_H

#include "ProseParser.h"
#include <unordered_map>
#include <memory>
#include <string>

class Interpreter {
private:
    // Symbol table for storing variable values
    using SymbolTable = std::unordered_map<std::string, int>;
    SymbolTable symbols;

    // Evaluate an AST node
    double evaluate(std::shared_ptr<ASTNode> node);

public:
    // Interpret the entire program (AST root node)
    void interpret(std::shared_ptr<ASTNode> program);
};

#endif // PROSEINTERPRETER_H
