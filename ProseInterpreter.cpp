#include "ProseInterpreter.h"
#include <stdexcept>
#include <iostream>

// Evaluate an AST Node
double Interpreter::evaluate(std::shared_ptr<ASTNode> node) {
    switch (node->type) {
    case ASTNodeType::PrintStatement: {
        double value = evaluate(node->children[0]);  // Evaluate the expression to print
        std::cout << value << std::endl;
        return value;
    }

    case ASTNodeType::Literal:
        // Check if the literal can be interpreted as an integer or floating point
        try {
            return std::stod(node->value);  // Convert to double if it is a floating point number
        }
        catch (const std::invalid_argument&) {
            return std::stoi(node->value);  // Fallback to integer if it's not a float
        }

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
        double left = evaluate(node->children[0]);
        double right = evaluate(node->children[1]);

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
        // Logical operators (you may need to cast the result to `bool` for logical operators)
        if (node->value == "and") return static_cast<bool>(left) && static_cast<bool>(right);
        if (node->value == "or") return static_cast<bool>(left) || static_cast<bool>(right);
        if (node->value == "not") return !static_cast<bool>(left);  // Negate the left operand using logical NOT (!)
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
