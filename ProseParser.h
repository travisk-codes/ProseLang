#ifndef PROSEPARSER_H
#define PROSEPARSER_H

#include "ProseLexer.h"
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>

// AST Node Types
enum class ASTNodeType {
    Function, // 0
    Identifier, // 1
    Assignment, // 2
    BinaryOperation, // 3
    Return, // 4
    Literal, // 5
    IfStatement,
    WhileLoop,
    ForLoop,
    Block, // 9
    PrintStatement,
};

// Abstract Syntax Tree Node
struct ASTNode {
    ASTNodeType type;
    std::string value;
    std::vector<std::shared_ptr<ASTNode>> children;

    ASTNode(ASTNodeType t, const std::string& v = "") : type(t), value(v) {}
};

// Parser class
class Parser {
private:
    std::vector<Token> tokens;
    size_t current;

    // Utility functions
    Token peek();
    Token consume(TokenType expectedType, const std::string& errorMessage);
    bool match(TokenType type);
    int getPrecedence(const std::string& op) const;

    // Parsing functions
    std::shared_ptr<ASTNode> parseStatement();
    std::shared_ptr<ASTNode> parseExpressionOrAssignment();
    std::shared_ptr<ASTNode> parseExpression(int precedence = 0);
    std::shared_ptr<ASTNode> parsePrimary();
    std::shared_ptr<ASTNode> parseBlock();
    std::shared_ptr<ASTNode> parseFunction();
    std::shared_ptr<ASTNode> parseIfStatement();
    std::shared_ptr<ASTNode> parseWhileLoop();
    std::shared_ptr<ASTNode> parseForLoop();
    std::shared_ptr<ASTNode> parseReturn();
    std::shared_ptr<ASTNode> parsePrintStatement();
    std::shared_ptr<ASTNode> parseIdentifier();
    std::shared_ptr<ASTNode> parseLiteral();

public:
    // Constructor
    Parser(const std::vector<Token>& tokenList);

    // Parse the full program
    std::shared_ptr<ASTNode> parseProgram();
};

#endif // PROSEPARSER_H
