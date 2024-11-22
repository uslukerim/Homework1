#include "LexicalAnalysis.h"

/**
 * @brief Constructor for the LexicalAnalysis class.
 * Initializes the tokenizer and data matrix.
 */
LexicalAnalysis::LexicalAnalysis(const Tokenizer& tokenizer, const CellMatrix& datain)
    : tokenizer(tokenizer), data(datain) {}

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
            std::string result = evaluateLabelFunction(token.value);
            if (result.find("Error") != std::string::npos) {
                return result; // Return the error message directly
            }
            values.push(result);
        } else if (token.type == TokenType::MatrixReference) {
            // Resolve the reference recursively
            std::string cellValue = getCellValue(token.value);
            if (cellValue.find("Error") != std::string::npos) {
                return cellValue; // Return the error message directly
            }
            values.push(cellValue);
        } else if (token.type == TokenType::Number) {
            values.push(token.value); // Push numeric tokens onto the values stack
        } else if (token.type == TokenType::Operator) {
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

    if (values.size() != 1) {
        return "Error: Invalid expression"; // Invalid formula
    }

    return values.top();
}

/**
 * @brief Evaluates a function label expression such as SUM, @SUM, STDDEV, or @STDDEV.
 */
std::string LexicalAnalysis::evaluateLabelFunction(const std::string& labelExpression) {
    std::regex labelRegex("(SUM|@SUM|STDDEV|@STDDEV|AVER|@AVER|MAX|@MAX|MIN|@MIN)\\(([A-Z]{1,2}[0-9]{1,3})\\.\\.([A-Z]{1,2}[0-9]{1,3})\\)");
    std::smatch match;

    if (std::regex_match(labelExpression, match, labelRegex)) {
        std::string label = match[1];      // Function label (SUM, @SUM, etc.)
        std::string startCell = match[2]; // Start cell
        std::string endCell = match[3];   // End cell

        // Normalize the label to ignore "@" prefix for consistency
        if (label[0] == '@') {
            label = label.substr(1); // Remove '@'
        }

        return calculateRangeFunction(label, startCell, endCell);
    }

    return "Error: Invalid formula label expression";
}


/**
 * @brief Calculates a function over a specified range of cells.
 */
std::string LexicalAnalysis::calculateRangeFunction(const std::string& label, const std::string& startCell, const std::string& endCell) {
    // Hücrelerin satır ve sütun koordinatlarını al
    int startCol = startCell[0] - 'A'; // Sütun
    int startRow = std::stoi(startCell.substr(1)) - 1; // Satır
    int endCol = endCell[0] - 'A'; // Sütun
    int endRow = std::stoi(endCell.substr(1)) - 1; // Satır

    // Geçersiz hücre aralığı kontrolü
    if (startRow < 0 || startRow >= data.getRows() || endRow < 0 || endRow >= data.getRows() ||
        startCol < 0 || startCol >= data.getCols() || endCol < 0 || endCol >= data.getCols()) {
        return "Error: Invalid cell range " + startCell + " to " + endCell;
    }

    // Hücrelerin aynı sütunda veya aynı satırda olup olmadığını kontrol et
    if (startCol != endCol && startRow != endRow) {
        return "Error: Function can only operate on the same column or row";
    }

    std::vector<std::string> values;

    // Aynı sütundaki hücreleri işle
    if (startCol == endCol) {
        for (int row = startRow; row <= endRow; ++row) {
            values.push_back(data(row,startCol));
        }
    }
    // Aynı satırdaki hücreleri işle
    else if (startRow == endRow) {
        for (int col = startCol; col <= endCol; ++col) {
            values.push_back(data(startRow,col));
        }
    }

    if (values.empty()) {
        return "Error: No valid cells in the specified range.";
    }

    // Sayısal değerleri ayıkla
    std::vector<double> doubleValues;
    for (const auto& val : values) {
        if (isNumeric(val)) {
            doubleValues.push_back(std::stod(val));
        }
    }

    // Fonksiyon hesaplama
    if (label == "SUM") {
        return std::to_string(std::accumulate(doubleValues.begin(), doubleValues.end(), 0.0));
    } else if (label == "AVER") {
        return std::to_string(std::accumulate(doubleValues.begin(), doubleValues.end(), 0.0) / doubleValues.size());
    } else if (label == "MAX") {
        return std::to_string(*std::max_element(doubleValues.begin(), doubleValues.end()));
    } else if (label == "MIN") {
        return std::to_string(*std::min_element(doubleValues.begin(), doubleValues.end()));
    } else if (label == "STDDEV") {
        // Ortalama hesapla
        double mean = std::accumulate(doubleValues.begin(), doubleValues.end(), 0.0) / doubleValues.size();

        // Varyans hesapla
        double variance = 0.0;
        for (const auto& val : doubleValues) {
            variance += (val - mean) * (val - mean);
        }
        variance /= doubleValues.size();

        // Standart sapma (karekök)
        return std::to_string(std::sqrt(variance));
    }

    return "Error: Unknown function label " + label;
}

/**
 * @brief Retrieves the value of a matrix cell based on its reference.
 */
std::string LexicalAnalysis::getCellValue(const std::string& cell) {
    int col = 0;
    size_t rowIndex = 0;

    // Calculate the column index (e.g., "A" -> 0, "AA" -> 26, "ZZ" -> 701)
    while (rowIndex < cell.size() && isalpha(cell[rowIndex])) {
        col = col * 26 + (cell[rowIndex] - 'A' + 1);
        rowIndex++;
    }
    col--; // Convert to 0-based indexing

    // Extract and calculate the row index
    int row = std::stoi(cell.substr(rowIndex)) - 1;

    // Validate that the row and column are within matrix bounds
    if (row < 0 || row >= data.getRows() || col < 0 || col >= data.getCols()) {
        return "Error: Invalid cell reference " + cell;
    }

    std::string cellContent = data(row,col);

    // If the cell content is a formula or reference, process it recursively
    std::vector<Token> tokens = tokenizer.tokenize(cellContent);
    if (!tokens.empty() && tokens[0].type == TokenType::Formula) {
        return evaluateFormula(tokens); // Process the formula recursively
    } else if (!tokens.empty()&&tokens.size()<=1 && tokens[0].type == TokenType::MatrixReference) {
        return getCellValue(tokens[0].value); // Resolve the reference recursively
    }
    else if (!tokens.empty() &&tokens.size()<=1&& tokens[0].type == TokenType::Number) {
        return formatDecimal(tokens[0].value); // Resolve the reference recursively
    }
   else if (!tokens.empty() &&tokens.size()>1) {//&& tokens[0].type == TokenType::Number
        return evaluateFormula(tokens); // Resolve the reference recursively
    }
    return cellContent; // Return raw value if it's not a formula or reference
}

/**
 * @brief Applies an arithmetic operation to two string values.
 */

std::string LexicalAnalysis::applyOp(const std::string& a, const std::string& b, char op) {
    if (!isNumeric(a) || !isNumeric(b)) {
        return "Error: Non-numeric value in operation";
    }

    double doubleA = std::stod(a);
    double doubleB = std::stod(b);

    switch (op) {
        case '+': return std::to_string(doubleA + doubleB);
        case '-': return std::to_string(doubleA - doubleB);
        case '*': return std::to_string(doubleA * doubleB);
        case '/':
            if (doubleB == 0) return "Error: Division by zero";
            return std::to_string(doubleA / doubleB);
        default:
            return "Error: Unknown operator";
    }
}

/**
 * @brief Checks if a string represents a numeric value.
 */
// bool LexicalAnalysis::isNumeric(const std::string& str) {
//     if (str.empty()) return false;
//     size_t start = (str[0] == '-') ? 1 : 0;
//     return std::all_of(str.begin() + start, str.end(), ::isdigit);
// }
bool LexicalAnalysis::isNumeric(const std::string& str) {
    return std::regex_match(str, std::regex("^-?\\d*\\.?\\d+([eE][-+]?\\d+)?$"));
}

/**
 * @brief Determines the precedence of an operator.
 */
int LexicalAnalysis::precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}
std::string LexicalAnalysis::formatDecimal(const std::string& number) {
double value = std::stod(number); // Convert string to double
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(15) << value; // Format with maximum precision
    std::string formatted = oss.str();

    // Trim trailing zeros
    size_t decimalPos = formatted.find('.');
    if (decimalPos != std::string::npos) {
        size_t end = formatted.find_last_not_of('0');
        if (end == decimalPos) {
            end--; // Remove the dot if no fractional part remains
        }
        formatted = formatted.substr(0, end + 1);
    }
    return formatted;
}
