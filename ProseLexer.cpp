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
            column = 0;
        }
        else {
            column++;
        }
        return c;
    }
    return '\0';
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
    std::string value;

    // Attempt to collect a multi-character operator or punctuation
    while (pos <= source.size()) {
        value = source.substr(start, pos - start + 1);

        // Check for multi-character operators
        if (operators.count(value)) {
            pos++;  // Consume the entire operator
            column += value.length();
            return { TokenType::Operator, value, line, column };
        }

        // If the next character is not alphabetic or punctuation, break
        if (!std::isalpha(source[pos]) && !std::ispunct(source[pos])) {
            break;
        }

        pos++;  // Expand to the next character
    }

    // If no operator matched, check for single-character punctuation
    value = source.substr(start, 1);
    if (punctuation.count(value)) {
        column++;
        return { TokenType::Punctuation, value, line, column };
    }

    // Unknown token fallback
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
    case TokenType::Keyword:        typeStr = "Keyword"; break;
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
        // std::cout << tokenToString(token) << std::endl;
    } while (token.type != TokenType::EndOfFile);
    return tokens;
}
