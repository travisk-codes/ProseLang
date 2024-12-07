#include "ProseInterpreter.h"
#include <stdexcept>
#include <iostream>

// Evaluate an AST Node
int Interpreter::evaluate(std::shared_ptr<ASTNode> node) {
    switch (node->type) {
    case ASTNodeType::PrintStatement: {
        int value = evaluate(node->children[0]);  // Evaluate the expression to print
        std::cout << value << std::endl;
        return value;
    }

    case ASTNodeType::Literal:
        return std::stoi(node->value);  // Return the literal value (1 or 0 for booleans)

    case ASTNodeType::Identifier: {
        auto it = symbols.find(node->value);
        if (it == symbols.end()) {
            throw std::runtime_error("Undefined variable: " + node->value);
        }
        return it->second;
    }

    case ASTNodeType::Assignment: {
        auto variable = node->children[0];
        auto value = evaluate(node->children[1]);
        symbols[variable->value] = value;  // Assign the value to the variable
        return value;
    }

    case ASTNodeType::BinaryOperation: {
        auto left = evaluate(node->children[0]);
        auto right = evaluate(node->children[1]);

        if (node->value == "+") return left + right;
        if (node->value == "-") return left - right;
        if (node->value == "*") return left * right;
        if (node->value == "/") {
            if (right == 0) throw std::runtime_error("Division by zero.");
            return left / right;
        }
        // Comparison operators
        if (node->value == "=") return left == right;
        if (node->value == ">") return left > right;
        if (node->value == "<") return left < right;
        if (node->value == ">=") return left >= right;
        if (node->value == "<=") return left <= right;
        // Logical operators
        if (node->value == "and") return left && right;
        if (node->value == "or") return left || right;
        if (node->value == "not") return !left;  // Negate the left operand using logical NOT (!)
        throw std::runtime_error("Unknown operator: " + node->value);
    }

    case ASTNodeType::Return:
        return evaluate(node->children[0]);

    default:
        throw std::runtime_error("Unsupported AST node type.");
    }
}

// Interpret the entire program
void Interpreter::interpret(std::shared_ptr<ASTNode> program) {
    for (auto& statement : program->children) {
        evaluate(statement);
    }
}
