#ifndef PROSEPARSER_H
#define PROSEPARSER_H

#include "ProseLexer.h"
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>

// AST Node Types
enum class ASTNodeType {
    Function,
    Identifier,
    Assignment,
    BinaryOperation,
    Return,
    Literal,
    IfStatement,
    WhileLoop,
    ForLoop,
    Block,
    PrintStatement,
    // No new node type for parentheses since they only group expressions
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

    // Parsing functions
    std::shared_ptr<ASTNode> parseStatement();
    std::shared_ptr<ASTNode> parseExpressionOrAssignment();
    std::shared_ptr<ASTNode> parseExpression(std::shared_ptr<ASTNode> left = nullptr);
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
