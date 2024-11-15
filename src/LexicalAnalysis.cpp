#include "LexicalAnalysis.h"

/**
 * @brief Constructor for the LexicalAnalysis class.
 * Initializes the tokenizer and data matrix.
 */
LexicalAnalysis::LexicalAnalysis(const Tokenizer& tokenizer, const std::vector<std::vector<std::string>>& data)
    : tokenizer(tokenizer), data(data) {}

/**
 * @brief Analyzes the input expression, tokenizes it, and evaluates the result.
 * 
 * @param input The string input to analyze.
 */
void LexicalAnalysis::analyze(const std::string& input) {
    std::vector<Token> tokens = tokenizer.tokenize(input);
    std::string result = evaluateFormula(tokens);

    if (result.find("Error") == std::string::npos) {
        std::cout << "Final result for " << input << " = " << result << std::endl;
    } else {
        std::cout << "Error in expression: " << result << std::endl;
    }
}

/**
 * @brief Evaluates a formula from tokens, operating on string values.
 * 
 * @param tokens The vector of tokens representing the formula.
 * @return std::string The result of the formula evaluation, or an error message.
 */
std::string LexicalAnalysis::evaluateFormula(const std::vector<Token>& tokens) {
    std::stack<std::string> values;
    std::stack<char> ops;

    for (const auto& token : tokens) {
        if (token.type == TokenType::Formula) {
            // Evaluate and push the result of the formula
            std::string result = evaluateLabelFunction(token.value);
            if (result.find("Error") != std::string::npos) {
                return result; // Return the error message directly
            }
            values.push(result);
        } else if (token.type == TokenType::MatrixReference) {
            // Get the value of the matrix reference and push it
            std::string cellValue = getCellValue(token.value);
            if (cellValue.find("Error") != std::string::npos) {
                return cellValue; // Return the error message directly
            }
            values.push(cellValue);
        } else if (token.type == TokenType::Number) {
            // Directly push numeric tokens onto the values stack
            values.push(token.value);
        } else if (token.type == TokenType::Operator) {
            // Process operators based on precedence
            while (!ops.empty() && precedence(ops.top()) >= precedence(token.value[0])) {
                if (values.size() < 2) {
                    return "Error: Invalid expression"; // Not enough operands
                }
                std::string val2 = values.top(); values.pop();
                std::string val1 = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOp(val1, val2, op));
            }
            ops.push(token.value[0]);
        } else {
            // Unknown token type
            return "Error: Unknown token type";
        }
    }

    // Process remaining operators in the stack
    while (!ops.empty()) {
        if (values.size() < 2) {
            return "Error: Invalid expression"; // Not enough operands
        }
        std::string val2 = values.top(); values.pop();
        std::string val1 = values.top(); values.pop();
        char op = ops.top(); ops.pop();
        values.push(applyOp(val1, val2, op));
    }

    // If there are no values or more than one value left, the expression is invalid
    if (values.size() != 1) {
        return "Error: Invalid expression"; // Invalid formula
    }

    return values.top();
}

/**
 * @brief Evaluates a function label expression such as SUM, AVER, MAX, or MIN.
 */
std::string LexicalAnalysis::evaluateLabelFunction(const std::string& labelExpression) {
    std::regex labelRegex("(SUM|AVER|MAX|MIN)\\(([A-Z]{1,2}[0-9]{1,3})\\.\\.([A-Z]{1,2}[0-9]{1,3})\\)");
    std::smatch match;

    if (std::regex_match(labelExpression, match, labelRegex)) {
        std::string label = match[1];      // Function label (SUM, AVER, etc.)
        std::string startCell = match[2]; // Start cell
        std::string endCell = match[3];   // End cell

        return calculateRangeFunction(label, startCell, endCell);
    }

    return "Error: Invalid formula label expression";
}

/**
 * @brief Calculates a function over a specified range of cells.
 */
std::string LexicalAnalysis::calculateRangeFunction(const std::string& label, const std::string& startCell, const std::string& endCell) {
    int startRow = startCell[0] - 'A';
    int startCol = std::stoi(startCell.substr(1)) - 1;
    int endRow = endCell[0] - 'A';
    int endCol = std::stoi(endCell.substr(1)) - 1;

    if (startRow < 0 || startRow >= data.size() || endRow < 0 || endRow >= data.size() ||
        startCol < 0 || startCol >= data[0].size() || endCol < 0 || endCol >= data[0].size()) {
        return "Error: Invalid cell range " + startCell + " to " + endCell;
    }

    std::vector<std::string> values;
    for (int row = startRow; row <= endRow; ++row) {
        for (int col = startCol; col <= endCol; ++col) {
            values.push_back(data[row][col]);
        }
    }

    if (values.empty()) {
        return "Error: No valid cells in the specified range.";
    }

    // Convert string values to integers and perform calculations
    std::vector<int> intValues;
    for (const auto& val : values) {
        if (isNumeric(val)) {
            intValues.push_back(std::stoi(val));
        }
    }

    if (label == "SUM") {
        return std::to_string(std::accumulate(intValues.begin(), intValues.end(), 0));
    } else if (label == "AVER") {
        return std::to_string(std::accumulate(intValues.begin(), intValues.end(), 0) / intValues.size());
    } else if (label == "MAX") {
        return std::to_string(*std::max_element(intValues.begin(), intValues.end()));
    } else if (label == "MIN") {
        return std::to_string(*std::min_element(intValues.begin(), intValues.end()));
    }

    return "Error: Unknown function label " + label;
}

/**
 * @brief Retrieves the value of a matrix cell based on its reference.
 */
std::string LexicalAnalysis::getCellValue(const std::string& cell) {
    int col = 0;
    size_t rowIndex = 0;

    while (rowIndex < cell.size() && isalpha(cell[rowIndex])) {
        col = col * 26 + (cell[rowIndex] - 'A' + 1);
        rowIndex++;
    }
    col--;

    int row = std::stoi(cell.substr(rowIndex)) - 1;

    if (row < 0 || row >= data.size() || col < 0 || col >= data[0].size()) {
        return "Error: Invalid cell reference " + cell;
    }

    return data[row][col];
}

/**
 * @brief Applies an arithmetic operation to two string values.
 */
std::string LexicalAnalysis::applyOp(const std::string& a, const std::string& b, char op) {
    if (!isNumeric(a) || !isNumeric(b)) {
        return "Error: Non-numeric value in operation";
    }

    int intA = std::stoi(a);
    int intB = std::stoi(b);

    switch (op) {
        case '+': return std::to_string(intA + intB);
        case '-': return std::to_string(intA - intB);
        case '*': return std::to_string(intA * intB);
        case '/':
            if (intB == 0) return "Error: Division by zero";
            return std::to_string(intA / intB);
        default:
            return "Error: Unknown operator";
    }
}

/**
 * @brief Checks if a string represents a numeric value.
 */
bool LexicalAnalysis::isNumeric(const std::string& str) {
    if (str.empty()) return false;
    size_t start = (str[0] == '-') ? 1 : 0;
    return std::all_of(str.begin() + start, str.end(), ::isdigit);
}

/**
 * @brief Determines the precedence of an operator.
 */
int LexicalAnalysis::precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}
