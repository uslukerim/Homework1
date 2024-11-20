

#include "Tokenizer.h"
#include <sstream>
#include <algorithm>
#include <numeric>

/**
 * @brief Constructor for the Tokenizer class.
 * Initializes the operators and formula labels.
 */
Tokenizer::Tokenizer(const std::vector<std::string>& operators, const std::vector<std::string>& formulaLabels)
    : operators(operators.begin(), operators.end()), formulaLabels(formulaLabels.begin(), formulaLabels.end()) {}

/**
 * @brief Tokenizes a string into tokens based on regular expressions and predefined operators/formula labels.
 */
std::vector<Token> Tokenizer::tokenize(const std::string& str) const {
    std::vector<Token> tokens;
    //std::regex tokenRegex("([A-Z][0-9]{1,3}|[\\+\\-\\*/]|(SUM|AVER|MAX|MIN)\\([A-Z][0-9]{1,3}\\.\\.[A-Z][0-9]{1,3}\\)|\\w+)");
    std::regex tokenRegex(
    "([A-Z][0-9]{1,3}|"                    // Hücre referansı: A1, B20, AA300 gibi
    "[\\+\\-\\*/]|"                        // Matematiksel operatörler: +, -, *, /
    "(SUM|AVER|MAX|MIN)\\([A-Z][0-9]{1,3}\\.\\.[A-Z][0-9]{1,3}\\)|" // Formüller: SUM(A1..A10)
    "-?\\d*\\.?\\d+([eE][-+]?\\d+)?|\\w+)" // Sayılar: 12.34, .1234, 1.23E4
);

    auto tokensBegin = std::sregex_iterator(str.begin(), str.end(), tokenRegex);
    auto tokensEnd = std::sregex_iterator();

    Token lastToken = { TokenType::Unknown, "" };

    for (std::sregex_iterator i = tokensBegin; i != tokensEnd; ++i) {
        std::string part = (*i).str();
        Token currentToken = classifyToken(part);

        // Check if current token is a MatrixReference and follows another MatrixReference without an operator
        if (lastToken.type == TokenType::MatrixReference && currentToken.type == TokenType::MatrixReference) {
            // Mark the entire sequence as a Label if two MatrixReferences appear consecutively without an operator
            currentToken.type = TokenType::Label;
            currentToken.value = lastToken.value + part;
            tokens.pop_back(); // Remove the last token since it's now part of a Label
        }
        else if (lastToken.type == TokenType::MatrixReference && currentToken.type != TokenType::Operator &&
                 currentToken.type != TokenType::Unknown) {
            // If any non-operator appears after a MatrixReference, mark it as a Label
            currentToken.type = TokenType::Label;
            currentToken.value = lastToken.value + part;
            tokens.pop_back(); // Remove the last token since it's now part of a Label
        }

        tokens.push_back(currentToken);
        lastToken = currentToken; // Update lastToken to the current one
    }

    return tokens;
}


/**
 * @brief Classifies individual string parts as specific token types.
 */
Token Tokenizer::classifyToken(const std::string& part) const {
    // Check for a MatrixReference: one or two letters followed by one to three digits
    if (std::regex_match(part, std::regex("^[A-Z]{1,2}[0-9]{1,3}$"))) {
        return { TokenType::MatrixReference, part };
    }
    // Check for a Formula pattern
    else if (std::regex_match(part, std::regex("^(SUM|AVER|MAX|MIN)\\([A-Z]{1,2}[0-9]{1,3}\\.\\.[A-Z]{1,2}[0-9]{1,3}\\)$"))) {
        return { TokenType::Formula, part };
    }
    // // Check for a Number
    // else if (std::regex_match(part, std::regex("^\\d+$"))) {
    //     return { TokenType::Number, part };
    // }
    else if (std::regex_match(part, std::regex("^-?\\.\\d+$"))) {
        std::string normalized = "0" + part; // Add leading 0 to numbers starting with '.'
        return { TokenType::Number, normalized };
    }

    // Check for a scientific notation number (e.g., .123E2 -> 0.123E2)
    else if (std::regex_match(part, std::regex("^-?\\.?\\d+([eE][-+]?\\d+)?$"))) {
        if (part[0] == '.') { // Handle numbers starting with '.'
            std::string normalized = "0" + part;
            return { TokenType::Number, normalized };
        }
        return { TokenType::Number, part };
    }
    // Mixed alphanumeric values are considered Label
    else if (std::regex_match(part, std::regex(".*[A-Za-z].*[0-9].*|.*[0-9].*[A-Za-z].*"))) {
        return { TokenType::Label, part };
    }
    // Check for an Operator
    else if (operators.find(part) != operators.end()) {
        return { TokenType::Operator, part };
    }
    return { TokenType::Unknown, part };
}




/**
 * @brief Helper function to convert a TokenType to a string for display purposes.
 */
std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::Number: return "Number";
        case TokenType::Label: return "Label";
        case TokenType::MatrixReference: return "MatrixReference";
        case TokenType::Formula: return "Formula";
        case TokenType::Operator: return "Operator";
        default: return "Unknown";
    }
}