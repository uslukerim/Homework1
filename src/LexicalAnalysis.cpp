#include "LexicalAnalysis.h"

/**
 * @brief Constructor for the LexicalAnalysis class.
 */
LexicalAnalysis::LexicalAnalysis(const Tokenizer& tokenizer, const std::vector<std::vector<int>>& data)
    : tokenizer(tokenizer), data(data) {}

/**
 * @brief Analyzes the input expression, tokenizes it, and evaluates the result.
 */
void LexicalAnalysis::analyze(const std::string& input) {
    std::vector<Token> tokens = tokenizer.tokenize(input);
    int result = evaluateFormula(tokens);

    if (result != -1) {
        std::cout << "Final result for " << input << " = " << result << std::endl;
    } else {
        std::cout << "Unknown Expression" << std::endl;
    }
}

/**
 * @brief Evaluates a sequence of tokens as a formula.
 */
int LexicalAnalysis::evaluateFormula(const std::vector<Token>& tokens) {
    std::stack<int> values;
    std::stack<char> ops;

    for (const auto& token : tokens) {
        if (token.type == TokenType::Formula) {
            int result = evaluateLabelFunction(token.value);
            values.push(result);
        } else if (token.type == TokenType::MatrixReference) {
            int cellValue = getCellValue(token.value);
            if (cellValue == -1) {
                std::cerr << "Evaluation stopped due to invalid cell reference: " << token.value << std::endl;
                return -1;
            }
            values.push(cellValue);
        } else if (token.type == TokenType::Number) {
            values.push(std::stoi(token.value)); // Push the numeric value onto the stack
        } else if (token.type == TokenType::Operator) {
            while (!ops.empty() && precedence(ops.top()) >= precedence(token.value[0])) {
                int val2 = values.top(); values.pop();
                int val1 = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOp(val1, val2, op));
            }
            ops.push(token.value[0]);
        } else {
            return -1; // Invalid token
        }
    }

    // Process remaining operators in the stack
    while (!ops.empty()) {
        int val2 = values.top(); values.pop();
        int val1 = values.top(); values.pop();
        char op = ops.top(); ops.pop();
        values.push(applyOp(val1, val2, op));
    }

    return values.top();
}

/**
 * @brief Evaluates a function label expression such as SUM, AVER, MAX, or MIN.
 */
int LexicalAnalysis::evaluateLabelFunction(const std::string& labelExpression) {
    std::regex labelRegex("(SUM|AVER|MAX|MIN)\\(([A-Z][0-9]{1,3})\\.\\.([A-Z][0-9]{1,3})\\)");
    std::smatch match;
    if (std::regex_match(labelExpression, match, labelRegex)) {
        std::string label = match[1];
        std::string startCell = match[2];
        std::string endCell = match[3];
        return calculateRangeFunction(label, startCell, endCell);
    }
    return 0;
}

/**
 * @brief Calculates a function over a specified range of cells.
 */
int LexicalAnalysis::calculateRangeFunction(const std::string& label, const std::string& startCell, const std::string& endCell) {
    int startRow = startCell[0] - 'A';
    int startCol = std::stoi(startCell.substr(1)) - 1;
    int endRow = endCell[0] - 'A';
    int endCol = std::stoi(endCell.substr(1)) - 1;

    if (startRow < 0 || startRow >= data.size() || endRow < 0 || endRow >= data.size() ||
        startCol < 0 || startCol >= data[0].size() || endCol < 0 || endCol >= data[0].size()) {
        std::cerr << "Error: Invalid cell range " << startCell << " to " << endCell << std::endl;
        return -1;
    }

    std::vector<int> values;
    for (int row = startRow; row <= endRow; ++row) {
        for (int col = startCol; col <= endCol; ++col) {
            values.push_back(data[row][col]);
        }
    }

    if (values.empty()) {
        std::cerr << "Error: No valid cells in the specified range." << std::endl;
        return -1;
    }

    if (label == "SUM") {
        return std::accumulate(values.begin(), values.end(), 0);
    } else if (label == "AVER") {
        return std::accumulate(values.begin(), values.end(), 0) / values.size();
    } else if (label == "MAX") {
        return *std::max_element(values.begin(), values.end());
    } else if (label == "MIN") {
        return *std::min_element(values.begin(), values.end());
    }

    return 0;
}

/**
 * @brief Retrieves the value of a specific cell in the matrix.
 */
int LexicalAnalysis::getCellValue(const std::string& cell) {
    int row = cell[0] - 'A';
    int col = std::stoi(cell.substr(1)) - 1;

    if (row < 0 || row >= data.size() || col < 0 || col >= data[0].size()) {
        std::cerr << "Error: Invalid cell reference " << cell << std::endl;
        return -1;
    }

    return data[row][col];
}

/**
 * @brief Applies an arithmetic operation between two integers.
 */
int LexicalAnalysis::applyOp(int a, int b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return b != 0 ? a / b : 0;
    }
    return 0;
}

/**
 * @brief Determines the precedence of an operator.
 */
int LexicalAnalysis::precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}
