#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <regex>

/**
 * @enum RegexType
 * @brief Defines categories of regex patterns used for token classification.
 */
enum class RegexType {
    TokenPattern,        ///< General pattern for token extraction.
    MatrixReference,     ///< Pattern for matrix references (e.g., A1, B10).
    Formula,             ///< Pattern for formulas (e.g., SUM(A1..A10)).
    DecimalNumber,       ///< Pattern for numbers starting with a decimal point (e.g., .123).
    GeneralNumber,       ///< Pattern for general numbers (e.g., 123, 1.23E4).
    AlphanumericLabel    ///< Pattern for alphanumeric labels (e.g., A1B2, Label123).
};

/**
 * @enum TokenType
 * @brief Defines types of tokens that can be identified in an input string.
 */
enum class TokenType { Number, Label, MatrixReference, Formula, Operator, Unknown };

/**
 * @struct Token
 * @brief Represents a single token with its type and value.
 */
struct Token {
    TokenType type; ///< The type of the token.
    std::string value; ///< The value of the token as a string.
};

/**
 * @class Tokenizer
 * @brief A class for splitting strings into meaningful tokens.
 */
class Tokenizer {
public:
    /**
     * @brief Constructs a Tokenizer with given operators, formula labels, and regex patterns.
     * @param operators A vector containing valid operator strings.
     * @param formulaLabels A vector containing valid formula label strings.
     * @param regexMap A map of regex patterns classified by RegexType.
     */
    Tokenizer(const std::vector<std::string>& operators,
              const std::vector<std::string>& formulaLabels,
              const std::unordered_map<RegexType, std::string>& regexMap);

    /**
     * @brief Splits a given input string into a sequence of tokens.
     * @param str The input string to be tokenized.
     * @return A vector of tokens representing parts of the input string.
     */
    std::vector<Token> tokenize(const std::string& str) const;

private:
    std::unordered_set<std::string> operators; ///< Set of valid operators for tokenization.
    std::unordered_set<std::string> formulaLabels; ///< Set of valid formula labels for tokenization.
    std::unordered_map<RegexType, std::regex> regexes; ///< Map of compiled regex patterns.

    /**
     * @brief Determines the type of a given string and classifies it as a token.
     * @param part A string segment to classify.
     * @return A Token containing the type and value of the input string.
     */
    Token classifyToken(const std::string& part) const;
};

/**
 * @brief Converts a TokenType enum to its string representation.
 * @param type The TokenType to convert.
 * @return A string representing the TokenType.
 */
std::string tokenTypeToString(TokenType type);

#endif // TOKENIZER_H
