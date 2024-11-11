#ifndef LEXICALANALYSIS_H
#define LEXICALANALYSIS_H

#include "Tokenizer.h"
#include <iostream>
#include <vector>
#include <stack>
#include <string>
#include <regex>
#include <numeric>

/**
 * @class LexicalAnalysis
 * @brief A class for analyzing and evaluating tokenized expressions within a matrix of data.
 */
class LexicalAnalysis {
public:
    /**
     * @brief Constructs a LexicalAnalysis instance.
     * @param tokenizer A Tokenizer object for tokenizing expressions
     * @param data A 2D matrix representing the data for cell references
     */
    LexicalAnalysis(const Tokenizer& tokenizer, const std::vector<std::vector<int>>& data);

    /**
     * @brief Analyzes and evaluates an input string expression.
     * @param input The string expression to analyze
     */
    void analyze(const std::string& input);

private:
    const Tokenizer& tokenizer; ///< Reference to the tokenizer
    const std::vector<std::vector<int>>& data; ///< Reference to the matrix data

    /**
     * @brief Evaluates a formula from a sequence of tokens.
     * @param tokens Vector of tokens to evaluate
     * @return The result of the formula evaluation
     */
    int evaluateFormula(const std::vector<Token>& tokens);

    /**
     * @brief Evaluates a formula function like SUM, AVER, MAX, or MIN.
     * @param labelExpression The expression containing the function label
     * @return The result of the function evaluation
     */
    int evaluateLabelFunction(const std::string& labelExpression);

    /**
     * @brief Calculates a range-based function (SUM, AVER, MAX, MIN) over specified cells.
     * @param label The function label (e.g., "SUM", "AVER")
     * @param startCell The starting cell in the range
     * @param endCell The ending cell in the range
     * @return The result of the range calculation
     */
    int calculateRangeFunction(const std::string& label, const std::string& startCell, const std::string& endCell);

    /**
     * @brief Retrieves the value from a specified cell in the matrix.
     * @param cell The cell reference string (e.g., "A1")
     * @return The cell value or -1 if the cell reference is invalid
     */
    int getCellValue(const std::string& cell);

    /**
     * @brief Applies an arithmetic operation between two operands.
     * @param a The first operand
     * @param b The second operand
     * @param op The operator character ('+', '-', '*', '/')
     * @return The result of the operation
     */
    int applyOp(int a, int b, char op);

    /**
     * @brief Determines the precedence level of an operator.
     * @param op The operator character ('+', '-', '*', '/')
     * @return The precedence level of the operator
     */
    int precedence(char op);
};

#endif // LEXICALANALYSIS_H
