#ifndef PROSELEXER_H
#define PROSELEXER_H

#include <string>
#include <vector>
#include <unordered_set>

// Token types
enum class TokenType {
    Keyword,
    Identifier,
    Number,
    StringLiteral,
    Operator,
    Punctuation,
    Unknown,
    EndOfFile
};

// Token structure
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

// Lexer class
class Lexer {
private:
    std::string source;
    size_t pos = 0;
    int line = 1;
    int column = 1;

    const std::unordered_set<std::string> keywords;
    const std::unordered_set<std::string> operators;
    const std::unordered_set<std::string> punctuation;

    char advance();
    char peek() const;
    void skipWhitespace();
    Token collectIdentifier();
    Token collectNumber();
    Token collectStringLiteral();
    Token collectOperatorOrPunctuation();

public:
    Lexer(const std::string& src);

    Token nextToken();
    std::vector<Token> tokenize();
};

#endif // PROSELEXER_H
