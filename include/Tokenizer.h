#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <regex>

/**
 * @enum TokenType
 * @brief Defines types of tokens that can be identified.
 */
enum class TokenType { Number, Label, MatrixReference, Formula, Operator, Unknown };

/**
 * @struct Token
 * @brief Represents a single token with a type and value.
 */
struct Token {
    TokenType type; ///< The type of the token
    std::string value; ///< The value of the token
};

/**
 * @class Tokenizer
 * @brief A class for tokenizing strings based on operators and formula labels.
 */
class Tokenizer {
public:
    /**
     * @brief Constructs a Tokenizer with given operators and formula labels.
     * @param operators List of valid operator strings
     * @param formulaLabels List of valid formula label strings
     */
    Tokenizer(const std::vector<std::string>& operators, const std::vector<std::string>& formulaLabels);

    /**
     * @brief Tokenizes a given string into a vector of tokens.
     * @param str The string to tokenize
     * @return A vector of tokens extracted from the string
     */
    std::vector<Token> tokenize(const std::string& str) const;

private:
    std::unordered_set<std::string> operators; ///< Set of valid operators
    std::unordered_set<std::string> formulaLabels; ///< Set of valid formula labels

    /**
     * @brief Classifies a string into a specific token type.
     * @param part The string part to classify
     * @return The classified Token
     */
    Token classifyToken(const std::string& part) const;
};

/**
 * @brief Converts a TokenType to a human-readable string.
 * @param type The TokenType to convert
 * @return A string representation of the TokenType
 */
std::string tokenTypeToString(TokenType type);

#endif // TOKENIZER_H

