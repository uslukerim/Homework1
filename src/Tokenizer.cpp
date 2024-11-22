#include "Tokenizer.h"
#include <sstream>
#include <algorithm>
#include <numeric>

/**
 * @brief Constructor for the Tokenizer class.
 * @param operators A vector of operator strings.
 * @param formulaLabels A vector of formula label strings.
 * @param regexMap A map of regex patterns classified by RegexType.
 */
Tokenizer::Tokenizer(const std::vector<std::string>& operators,
                     const std::vector<std::string>& formulaLabels,
                     const std::unordered_map<RegexType, std::string>& regexMap)
    : operators(operators.begin(), operators.end()),
      formulaLabels(formulaLabels.begin(), formulaLabels.end()) {
    // Convert regex strings to std::regex and store them
    for (const auto& [type, pattern] : regexMap) {
        regexes[type] = std::regex(pattern);
    }
}

/**
 * @brief Tokenizes the input string into a list of tokens based on regex patterns and predefined rules.
 * @param str The input string to tokenize.
 * @return A vector of Token objects extracted from the input string.
 */
std::vector<Token> Tokenizer::tokenize(const std::string& str) const {
    std::vector<Token> tokens;

    if (regexes.find(RegexType::TokenPattern) == regexes.end()) {
        throw std::runtime_error("TokenPattern regex is not defined.");
    }

    // Use the TokenPattern regex for initial token matching
    std::regex tokenRegex = regexes.at(RegexType::TokenPattern);

    auto tokensBegin = std::sregex_iterator(str.begin(), str.end(), tokenRegex);
    auto tokensEnd = std::sregex_iterator();

    Token lastToken = { TokenType::Unknown, "" };

    for (std::sregex_iterator i = tokensBegin; i != tokensEnd; ++i) {
        std::string part = (*i).str();
        Token currentToken = classifyToken(part);

        if (lastToken.type == TokenType::MatrixReference && currentToken.type == TokenType::MatrixReference) {
            currentToken.type = TokenType::Label;
            currentToken.value = lastToken.value + part;
            tokens.pop_back();
        } else if (lastToken.type == TokenType::MatrixReference && currentToken.type != TokenType::Operator &&
                   currentToken.type != TokenType::Unknown) {
            currentToken.type = TokenType::Label;
            currentToken.value = lastToken.value + part;
            tokens.pop_back();
        }

        tokens.push_back(currentToken);
        lastToken = currentToken;
    }

    return tokens;
}

/**
 * @brief Determines the type of a string segment and creates a corresponding token.
 * @param part The string segment to classify.
 * @return A Token object containing the type and value of the input segment.
 */
Token Tokenizer::classifyToken(const std::string& part) const {
    if (std::regex_match(part, regexes.at(RegexType::MatrixReference))) {
        return { TokenType::MatrixReference, part };
    } else if (std::regex_match(part, regexes.at(RegexType::Formula))) {
        return { TokenType::Formula, part };
    } else if (std::regex_match(part, regexes.at(RegexType::DecimalNumber))) {
        std::string normalized = "0" + part;
        return { TokenType::Number, normalized };
    } else if (std::regex_match(part, regexes.at(RegexType::GeneralNumber))) {
        return { TokenType::Number, part };
    } else if (std::regex_match(part, regexes.at(RegexType::AlphanumericLabel))) {
        return { TokenType::Label, part };
    } else if (operators.find(part) != operators.end()) {
        return { TokenType::Operator, part };
    }
    return { TokenType::Unknown, part };
}

/**
 * @brief Converts a TokenType enum to its string representation.
 * @param type The TokenType to convert.
 * @return A string representation of the TokenType.
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
