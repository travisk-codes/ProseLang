#include "ProseParser.h"
#include <iostream>

// Constructor
Parser::Parser(const std::vector<Token>& tokenList) : tokens(tokenList), current(0) {}

// Utility: Peek at the current token
Token Parser::peek() {
    if (current < tokens.size()) {
        return tokens[current];
    }
    else {
        return Token{ TokenType::EndOfFile, "", 0, 0 };
    }
}

// Utility: Consume a token if it matches the expected type
Token Parser::consume(TokenType expectedType, const std::string& errorMessage) {
    if (peek().type == expectedType) {
        return tokens[current++];
    }
    throw std::runtime_error(errorMessage);
}

// Utility: Match and advance if the token matches a type
bool Parser::match(TokenType type) {
    if (peek().type == type) {
        current++;
        return true;
    }
    return false;
}

// Parse the entire program
std::shared_ptr<ASTNode> Parser::parseProgram() {
    return parseBlock();
}

// Parse a block of statements
std::shared_ptr<ASTNode> Parser::parseBlock() {
    auto blockNode = std::make_shared<ASTNode>(ASTNodeType::Block);
    while (peek().type != TokenType::EndOfFile) {
        blockNode->children.push_back(parseStatement());
    }
    return blockNode;
}

// Parse a single statement
std::shared_ptr<ASTNode> Parser::parseStatement() {
    if (peek().type == TokenType::Keyword) {
        const std::string& keyword = peek().value;
        if (keyword == "function") return parseFunction();
        if (keyword == "if") return parseIfStatement();
        if (keyword == "while") return parseWhileLoop();
        if (keyword == "return") return parseReturn();
        if (keyword == "print") return parsePrintStatement();
        if (keyword == "for") return parseForLoop();
    }
    return parseExpressionOrAssignment();
}

// Parse an expression or assignment
std::shared_ptr<ASTNode> Parser::parseExpressionOrAssignment() {
    auto left = parsePrimary(); // Parse the left-hand side (identifier or literal)

    // Check if the next token is "is" (assignment keyword)
    if (peek().type == TokenType::Keyword && peek().value == "is") {
        consume(TokenType::Keyword, "Expected 'is' in assignment.");
        auto value = parseExpression(); // Parse the right-hand side of the assignment
        auto assignmentNode = std::make_shared<ASTNode>(ASTNodeType::Assignment);
        assignmentNode->children.push_back(left);  // Variable being assigned
        assignmentNode->children.push_back(value); // Value to assign
        return assignmentNode;
    }

    // If not an assignment, parse it as a general expression
    return parseExpression(left);
}

// Parse an expression
std::shared_ptr<ASTNode> Parser::parseExpression(std::shared_ptr<ASTNode> left) {
    if (!left) left = parsePrimary(); // Parse the initial operand (literal, identifier)

    // Handle the "not" operator (highest precedence for logical negation)
    if (peek().type == TokenType::Keyword && peek().value == "not") {
        consume(TokenType::Keyword, "Expected 'not' operator.");
        auto right = parsePrimary();
        auto notOpNode = std::make_shared<ASTNode>(ASTNodeType::BinaryOperation, "not");
        notOpNode->children.push_back(left);  // Left operand (will be negated)
        notOpNode->children.push_back(right); // Right operand is parsed after "not"
        left = notOpNode;  // Update left operand for negation
    }


    // Handle comparison operators (lower precedence)
    while (peek().type == TokenType::Operator &&
        (peek().value == "=" || peek().value == ">" || peek().value == "<" ||
            peek().value == ">=" || peek().value == "<=")) {
        Token op = consume(TokenType::Operator, "Expected a comparison operator.");
        auto right = parsePrimary();
        auto comparisonOpNode = std::make_shared<ASTNode>(ASTNodeType::BinaryOperation, op.value);
        comparisonOpNode->children.push_back(left);
        comparisonOpNode->children.push_back(right);
        left = comparisonOpNode;  // Update left operand for chained comparisons
    }

    // Handle logical operators (and, or)
    while (peek().type == TokenType::Operator &&
        (peek().value == "and" || peek().value == "or")) {
        Token op = consume(TokenType::Operator, "Expected a logical operator.");
        auto right = parsePrimary();
        auto logicalOpNode = std::make_shared<ASTNode>(ASTNodeType::BinaryOperation, op.value);
        logicalOpNode->children.push_back(left);
        logicalOpNode->children.push_back(right);
        left = logicalOpNode;  // Update left operand for chained logical operations
    }


    // Handle arithmetic operators (highest precedence)
    while (peek().type == TokenType::Operator &&
        (peek().value == "+" || peek().value == "-" ||
            peek().value == "*" || peek().value == "/")) {
        Token op = consume(TokenType::Operator, "Expected an arithmetic operator.");
        auto right = parsePrimary();
        auto arithmeticOpNode = std::make_shared<ASTNode>(ASTNodeType::BinaryOperation, op.value);
        arithmeticOpNode->children.push_back(left);
        arithmeticOpNode->children.push_back(right);
        left = arithmeticOpNode;  // Update left operand for chained arithmetic operations
    }

    return left;
}

// Parse a primary expression
std::shared_ptr<ASTNode> Parser::parsePrimary() {
    if (peek().type == TokenType::Number) {
        Token token = consume(TokenType::Number, "Expected a number.");
        return std::make_shared<ASTNode>(ASTNodeType::Literal, token.value);
    }

    else if (peek().type == TokenType::Identifier) {
        return parseIdentifier();
    }
    else if (peek().type == TokenType::Keyword) {
        // Handle boolean literals
        if (peek().value == "true") {
            consume(TokenType::Keyword, "Expected 'true'.");
            return std::make_shared<ASTNode>(ASTNodeType::Literal, "1");  // true is represented as 1
        }
        else if (peek().value == "false") {
            consume(TokenType::Keyword, "Expected 'false'.");
            return std::make_shared<ASTNode>(ASTNodeType::Literal, "0");  // false is represented as 0
        }
    }
    else if (peek().type == TokenType::Punctuation && peek().value == "(") {
        // Handle parenthesized expressions
        consume(TokenType::Punctuation, "Expected '('.");
        auto expr = parseExpression();  // Parse the expression inside the parentheses
        consume(TokenType::Punctuation, "Expected ')'.");  // Ensure closing parenthesis
        return expr;  // Return the grouped expression
    }

    throw std::runtime_error("Unexpected token in expression.");
}

// Parse an identifier
std::shared_ptr<ASTNode> Parser::parseIdentifier() {
    Token token = consume(TokenType::Identifier, "Expected an identifier.");
    return std::make_shared<ASTNode>(ASTNodeType::Identifier, token.value);
}

// Parse a literal
std::shared_ptr<ASTNode> Parser::parseLiteral() {
    Token token = consume(TokenType::Number, "Expected a literal.");
    return std::make_shared<ASTNode>(ASTNodeType::Literal, token.value);
}

// Parse a function
std::shared_ptr<ASTNode> Parser::parseFunction() {
    consume(TokenType::Keyword, "Expected 'function'.");
    auto functionName = parseIdentifier();
    consume(TokenType::Keyword, "Expected 'takes'.");
    auto parameter = parseIdentifier();
    auto functionNode = std::make_shared<ASTNode>(ASTNodeType::Function, functionName->value);
    functionNode->children.push_back(parameter);
    functionNode->children.push_back(parseBlock());
    return functionNode;
}

// Parse an if statement
std::shared_ptr<ASTNode> Parser::parseIfStatement() {
    consume(TokenType::Keyword, "Expected 'if'.");
    auto condition = parseExpression();
    auto ifNode = std::make_shared<ASTNode>(ASTNodeType::IfStatement);
    ifNode->children.push_back(condition);
    ifNode->children.push_back(parseBlock());
    return ifNode;
}

// Parse a while loop
std::shared_ptr<ASTNode> Parser::parseWhileLoop() {
    consume(TokenType::Keyword, "Expected 'while'.");
    auto condition = parseExpression();
    auto whileNode = std::make_shared<ASTNode>(ASTNodeType::WhileLoop);
    whileNode->children.push_back(condition);
    whileNode->children.push_back(parseBlock());
    return whileNode;
}

// Parse a for loop
std::shared_ptr<ASTNode> Parser::parseForLoop() {
    consume(TokenType::Keyword, "Expected 'for'.");
    auto iterator = parseIdentifier();
    consume(TokenType::Keyword, "Expected 'is'.");
    auto start = parseExpression();
    consume(TokenType::Keyword, "Expected 'to'.");
    auto end = parseExpression();
    auto forNode = std::make_shared<ASTNode>(ASTNodeType::ForLoop, iterator->value);
    forNode->children.push_back(start);
    forNode->children.push_back(end);
    forNode->children.push_back(parseBlock());
    return forNode;
}

// Parse a return statement
std::shared_ptr<ASTNode> Parser::parseReturn() {
    consume(TokenType::Keyword, "Expected 'return'.");
    auto value = parseExpression();
    auto returnNode = std::make_shared<ASTNode>(ASTNodeType::Return);
    returnNode->children.push_back(value);
    return returnNode;
}

// Parse a print statement
std::shared_ptr<ASTNode> Parser::parsePrintStatement() {
    consume(TokenType::Keyword, "Expected 'print'.");
    auto value = parseExpression(); // Parse the value to print
    auto printNode = std::make_shared<ASTNode>(ASTNodeType::PrintStatement);
    printNode->children.push_back(value);
    return printNode;
}
