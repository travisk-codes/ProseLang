#include "ProseLexer.h"
#include <cctype>
#include <iostream>

// Lexer constructor
Lexer::Lexer(const std::string& src)
    : source(src),
    keywords({ "function", "return", "print", "if", "else", "for", "while", "is", "to", "true", "false" }),
    operators({ "and", "or", "not", "=", "<=", ">=", "<", ">", "+", "-", "*", "/" }),
    punctuation({ ":", "(", ")" }) {
}

// Advances to the next character
char Lexer::advance() {
    if (pos < source.size()) {
        char c = source[pos++];
        if (c == '\n') {
            line++;
            column = 1;  // Reset column to 1 on a new line
        }
        else {
            column++;  // Increment column position
        }
        return c;
    }
    return '\0';  // Return null character when end of string is reached
}

// Peeks at the current character without consuming it
char Lexer::peek() const {
    return (pos < source.size()) ? source[pos] : '\0';
}

// Skips whitespace
void Lexer::skipWhitespace() {
    while (isspace(peek())) advance();
}

// Collects an identifier or keyword
Token Lexer::collectIdentifier() {
    size_t start = pos - 1;
    while (isalnum(peek()) || peek() == '_') advance();
    std::string value = source.substr(start, pos - start);
    return { keywords.count(value) ? TokenType::Keyword : TokenType::Identifier, value, line, column };
}

// Collects a number
Token Lexer::collectNumber() {
    size_t start = pos - 1;
    while (isdigit(peek())) advance();
    return { TokenType::Number, source.substr(start, pos - start), line, column };
}

// Collects a string literal
Token Lexer::collectStringLiteral() {
    size_t start = pos;
    std::string value;
    while (peek() != '"' && peek() != '\0') value += advance();
    if (peek() == '"') advance();
    return { TokenType::StringLiteral, value, line, column };
}

// Collects an operator or punctuation
Token Lexer::collectOperatorOrPunctuation() {
    size_t start = pos - 1;  // Start of the current token
    std::string value = source.substr(start, pos - start);

    // Check for punctuation (like parentheses, colons, etc.)
    if (punctuation.count(value)) {
        return { TokenType::Punctuation, value, line, column };
    }

    // Check for multi-character operators first
    while (pos < source.size()) {
        // Check for multi-character operators
        if (operators.count(value)) {
            column += value.length();  // Move column position forward by the length of the operator
            return { TokenType::Operator, value, line, column };
        }

        // If not a valid operator, check if the character is not part of an operator (like alphabetic or punctuation)
        if (!std::isalpha(source[pos]) && !std::ispunct(source[pos])) {
            break;
        }

        pos++;  // Expand to the next character
    }

    // Check for single-character operators (like '+', '-', etc.)
    value = source.substr(start, 1);
    if (operators.count(value)) {
        return { TokenType::Operator, value, line, column };
    }

    // If no operator or punctuation matched, return Unknown
    return { TokenType::Unknown, value, line, column };
}

// Generates the next token
Token Lexer::nextToken() {
    skipWhitespace();
    if (pos >= source.size()) return { TokenType::EndOfFile, "", line, column };
    char c = advance();
    if (c == '"') return collectStringLiteral();
    if (isalpha(c) || c == '_') return collectIdentifier();
    if (isdigit(c)) return collectNumber();
    if (ispunct(c)) return collectOperatorOrPunctuation();
    return { TokenType::Unknown, std::string(1, c), line, column };
}

std::string tokenToString(const Token& token) {
    std::string typeStr;

    switch (token.type) {
    case TokenType::Keyword:       typeStr = "Keyword"; break;
    case TokenType::Identifier:    typeStr = "Identifier"; break;
    case TokenType::Number:        typeStr = "Number"; break;
    case TokenType::StringLiteral: typeStr = "StringLiteral"; break;
    case TokenType::Operator:      typeStr = "Operator"; break;
    case TokenType::Punctuation:   typeStr = "Punctuation"; break;
    case TokenType::Unknown:       typeStr = "Unknown"; break;
    case TokenType::EndOfFile:     typeStr = "EndOfFile"; break;
    default:                       typeStr = "Invalid"; break;
    }

    return "Type: " + typeStr + ", Value: \"" + token.value + "\", Line: " + std::to_string(token.line) + ", Column: " + std::to_string(token.column);
}


// Tokenizes the entire source code
std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    Token token;
    do {
        token = nextToken();
        tokens.push_back(token);
        //std::cout << tokenToString(token) << std::endl;
    } while (token.type != TokenType::EndOfFile);
    return tokens;
}
