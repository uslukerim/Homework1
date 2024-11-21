#ifndef LEXICAL_ANALYSIS_H
#define LEXICAL_ANALYSIS_H

#include <iostream>
#include <vector>
#include <stack>
#include <string>
#include <regex>
#include <numeric>
#include <algorithm>
#include <sstream>  // std::ostringstream için
#include <iomanip>
#include <cmath>
#include "Tokenizer.h" // Tokenizer class is assumed to be implemented separately


/**
 * @brief The LexicalAnalysis class for analyzing and evaluating formulas and expressions.
 */
class LexicalAnalysis {
public:
    /**
     * @brief Constructor for the LexicalAnalysis class.
     * 
     * @param tokenizer A reference to the Tokenizer instance for tokenizing strings.
     * @param data A 2D vector of strings representing the spreadsheet data.
     */
    LexicalAnalysis(const Tokenizer& tokenizer, const std::vector<std::vector<std::string>>& data);

    /**
     * @brief Analyzes the input string, tokenizes it, and evaluates the result.
     * 
     * @param input The input string to analyze.
     */
    void analyze(const std::string& input);

    /**
     * @brief Evaluates a formula based on a vector of tokens.
     * 
     * @param tokens The vector of tokens representing the formula.
     * @return std::string The result of the formula evaluation.
     */
    std::string evaluateFormula(const std::vector<Token>& tokens);

    /**
     * @brief Retrieves the value of a matrix cell based on its reference.
     * 
     * @param cell The string representing the cell reference (e.g., "A1").
     * @return std::string The value of the cell.
     */
    std::string getCellValue(const std::string& cell);

    /**
     * @brief Evaluates a function label expression such as SUM, AVER, MAX, or MIN.
     * 
     * @param labelExpression The string containing the function label expression.
     * @return std::string The result of the evaluation.
     */
    std::string evaluateLabelFunction(const std::string& labelExpression);

    /**
     * @brief Calculates a function over a specified range of cells.
     * 
     * @param label The function label (SUM, AVER, etc.).
     * @param startCell The starting cell reference.
     * @param endCell The ending cell reference.
     * @return std::string The result of the calculation.
     */
    std::string calculateRangeFunction(const std::string& label, const std::string& startCell, const std::string& endCell);
    
    /**
     * @brief Checks if a string represents a numeric value.
     * 
     * @param str The string to check.
     * @return bool True if the string is numeric, false otherwise.
     */
    bool isNumeric(const std::string& str);

    std::string  formatDecimal(const std::string& number) ;

private:
    const Tokenizer& tokenizer; ///< Reference to the Tokenizer instance.
    const std::vector<std::vector<std::string>>& data; ///< Spreadsheet data.

    /**
     * @brief Applies an arithmetic operation to two string values.
     * 
     * @param a The first operand as a string.
     * @param b The second operand as a string.
     * @param op The operator (+, -, *, /).
     * @return std::string The result of the operation.
     */
    std::string applyOp(const std::string& a, const std::string& b, char op);


    /**
     * @brief Determines the precedence of an operator.
     * 
     * @param op The operator character.
     * @return int The precedence of the operator.
     */
    int precedence(char op);
};

#endif // LEXICAL_ANALYSIS_H
