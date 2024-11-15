#include "Spreadsheet.h"
#include "AnsiTerminal.h"
#include <iostream>
#include <iomanip>
#include <string>

#include <algorithm>

// Constructor: Initializes the spreadsheet with the specified number of rows and columns
Spreadsheet::Spreadsheet(int rows, int cols) : rows(rows), cols(cols) {
    data.resize(rows, std::vector<std::string>(cols, " ")); // Initialize all cells with empty strings
secondHeader =" ";
}
/**
 * @brief Ensures the spreadsheet size can accommodate the given row and column.
 * 
 * Dynamically resizes the spreadsheet if the given row or column exceeds
 * the current dimensions. New cells are initialized as empty strings.
 * 
 * @param newRow The 1-based row index that needs to be accommodated.
 * @param newCol The 1-based column index that needs to be accommodated.
 */
void Spreadsheet::resizeIfNeeded(int newRow, int newCol) {
    // Resize rows if necessary
    if (newRow > rows) {
        data.resize(newRow, std::vector<std::string>(cols, " ")); // Add new rows
        rows = newRow;
    }

    // Resize columns if necessary
    if (newCol > cols) {
        for (auto& row : data) {
            row.resize(newCol, ""); // Add new columns to each row
        }
        cols = newCol;
    }
}
/**
 * @brief Sets the value of a specific cell in the spreadsheet.
 * 
 * Automatically resizes the spreadsheet if the given row or column index
 * exceeds the current dimensions. Filters out newline characters (`\n`)
 * from the input value.
 * 
 * @param row The 1-based row index of the cell.
 * @param col The 1-based column index of the cell.
 * @param value The value to assign to the specified cell.
 */
void Spreadsheet::setValue(int row, int col, const std::string& value) {
    if (row >= 1 && col >= 1) { // Ensure valid 1-based indexing
        resizeIfNeeded(row, col); // Resize if necessary

        // Remove newline characters from the input value
        std::string filteredValue = value;
        filteredValue.erase(std::remove(filteredValue.begin(), filteredValue.end(), '\n'), filteredValue.end());

        // Assign the filtered value to the cell
        data[row - 1][col - 1] = filteredValue; // 0-based indexing internally
    }
}
// @TODO: its should be change
void Spreadsheet::setFirstHeader(int row, int col, const std::string& value) {
    if (row >= 1 && row <= rows && col >= 1 && col <= cols) {
        data[row - 1][col - 1] = value; // Store value in 0-based indexing

        
    }
}
std::string Spreadsheet::getColumnLabel(int index) const{
    std::string label;
    while (index >= 0) {
        label = char('A' + index % 26) + label;
        index = index / 26 - 1;
    }
    return label;
}
// Set the value of a specific cell
void Spreadsheet::setSecondHeader(const std::string& value) {
    if(value ==" "){
        secondHeader =" ";
    }else{
        if(getContentType(value)=='V')secondHeader="Value:"+value;else secondHeader="Label:"+value;
       // secondHeader= " \033[42m" + (getContentType(value)=='V')?"Value:":"Label:" + value;
    }
}

// Get the value of a specific cell
std::string Spreadsheet::getValue(int row, int col) const {
    if (row >= 1 && row <= rows && col >= 1 && col <= cols) {
        return data[row - 1][col - 1]; // Retrieve value using 0-based indexing
    }
    return ""; // Return an empty string if out of bounds
}
char Spreadsheet::getContentType(const std::string& content) const {
bool isNumeric = true;

    // İçerikteki her karakteri kontrol et
    for (char ch : content) {
        if (!std::isdigit(ch) && ch != '.') { // Sayısal karakter veya ondalık nokta değilse
            isNumeric = false;
            break;
        }
    }

    // Sayısal ise 'V', değilse 'L' döndür
    return isNumeric ? 'V' : 'L';
}

void Spreadsheet::display(AnsiTerminal& terminal,int cursorRow, int cursorCol, int offsetRow,int offsetCol) {
    terminal.clearScreen();

    const int headerRow = 4;
    const int headerCol = 8;
    const int cellWidth = 10;

// Display selected cell info
    std::string cellLabel =  getColumnLabel(cursorCol) + std::to_string(cursorRow + 1);
    std::string cellContent = getValue(cursorRow + 1, cursorCol + 1); // Adjust to 1-based indexing
    std::string displayContent = cellContent.empty() ? " " : cellContent;
    terminal.printAt(1, 2, "\033[42m " + cellLabel + " (" + std::string(1, getContentType(displayContent)) + ") " + displayContent + " \033[0m");

    // Tokenize and analyze the selected cell

    std::vector<std::string> operators = { "+", "-", "*", "/" };
    std::vector<std::string> formulaLabels = { "SUM", "AVER", "MAX", "MIN" };

    Tokenizer tokenizer(operators, formulaLabels);

    LexicalAnalysis lexicalAnalyzer(tokenizer,data);




    // Display second header
    terminal.printAt(2, 2, secondHeader);    // 10x10 pencere içindeki sütun başlıklarını çiz
    for (int c = 0; c < windowSize && c + offsetCol < cols; ++c) {
        std::string colLabel = getColumnLabel(c + offsetCol);
        colLabel.resize(10,' ');
        terminal.printAt(headerRow, headerCol + c * cellWidth, "\033[42m " + colLabel + " \033[0m");
    }

    // 10x10 pencere içindeki satır başlıklarını çiz
    for (int r = 0; r < windowSize && r + offsetRow < rows; ++r) {
        terminal.printAt(headerRow + r + 1, 2, "\033[42m " + std::to_string(r + offsetRow + 1) + " \033[0m");
    }

    // // Display second header
    // terminal.printAt(2, 2, secondHeader);

    // // Draw column headers within the 10x10 window
    // for (int c = 0; c < windowSize && c + offsetCol < cols; ++c) {
    //     std::string colLabel = getColumnLabel(c + offsetCol);
    //     colLabel.resize(10, ' ');
    //     terminal.printAt(headerRow, headerCol + c * cellWidth, "\033[42m " + colLabel + " \033[0m");
    // }

    // Draw row headers within the 10x10 window
    for (int r = 0; r < windowSize && r + offsetRow < rows; ++r) {
        terminal.printAt(headerRow + r + 1, 2, "\033[42m " + std::to_string(r + offsetRow + 1) + " \033[0m");
    }

    for (int r = 0; r < windowSize && r + offsetRow < rows; ++r) {
        for (int c = 0; c < windowSize && c + offsetCol < cols; ++c) {
            int rowPosition = headerRow + r + 1;
            int colPosition = headerCol + c * cellWidth;

            // Get cell content
            std::string cellContent = data[r + offsetRow][c + offsetCol];
            cellContent.resize(10, ' '); // Ensure fixed width for display

            // Analyze the current cell
            std::vector<Token> cellTokens = tokenizer.tokenize(data[r + offsetRow][c + offsetCol]);
             // Check for empty tokens or unknown token types
            std::string analyzedValue;
            if (cellTokens.empty() || cellTokens[0].type == TokenType::Unknown) {
                analyzedValue = cellContent.substr(0, 8); // Display raw content
            } else {
                analyzedValue = lexicalAnalyzer.evaluateFormula(cellTokens);
            }

            // Determine display text based on analysis
            std::string displayText = analyzedValue;
            if (analyzedValue.find("Error") != std::string::npos) {
                displayText = " "; // Display "Err" for invalid formulas or references
            }
            displayText.resize(10, ' '); // Ensure fixed width for display

            // Highlight the active cell
            if (r + offsetRow == cursorRow && c + offsetCol == cursorCol) {
                terminal.printInvertedAt(rowPosition, colPosition + 1, displayText);
            } else {
                terminal.printAt(rowPosition, colPosition + 1, " " + displayText + " ");
            }
        }
    }

    std::cout << "\033[0m" << std::flush;

}


// void Spreadsheet::display(AnsiTerminal& terminal, int cursorRow, int cursorCol, int offsetRow, int offsetCol) const {
//     terminal.clearScreen();

//     const int headerRow = 4;
//     const int headerCol = 8;
//     const int cellWidth = 10;
// // Display selected cell info
//     std::string cellLabel =  getColumnLabel(cursorCol) + std::to_string(cursorRow + 1);
//     std::string cellContent = getValue(cursorRow + 1, cursorCol + 1); // Adjust to 1-based indexing
//     std::string displayContent = cellContent.empty() ? " " : cellContent;
//     terminal.printAt(1, 2, "\033[42m " + cellLabel + " (" + std::string(1, getContentType(displayContent)) + ") " + displayContent + " \033[0m");

//     // Display second header
//     terminal.printAt(2, 2, secondHeader);    // 10x10 pencere içindeki sütun başlıklarını çiz
//     for (int c = 0; c < windowSize && c + offsetCol < cols; ++c) {
//         std::string colLabel = getColumnLabel(c + offsetCol);
//         colLabel.resize(10,' ');
//         terminal.printAt(headerRow, headerCol + c * cellWidth, "\033[42m " + colLabel + " \033[0m");
//     }

//     // 10x10 pencere içindeki satır başlıklarını çiz
//     for (int r = 0; r < windowSize && r + offsetRow < rows; ++r) {
//         terminal.printAt(headerRow + r + 1, 2, "\033[42m " + std::to_string(r + offsetRow + 1) + " \033[0m");
//     }

//     // 10x10 pencere içindeki hücreleri çiz
//     for (int r = 0; r < windowSize && r + offsetRow < rows; ++r) {
//         for (int c = 0; c < windowSize && c + offsetCol < cols; ++c) {
//             int rowPosition = headerRow + r + 1;
//             int colPosition = headerCol + c * cellWidth;
//             std::string cellText = data[r + offsetRow][c + offsetCol].substr(0, 8);
//             cellText.resize(10, ' '); // Diziyi 10 karaktere genişlet, gerekirse boşluk ile doldur

//             // Aktif hücreyi vurgula
//             if (r + offsetRow == cursorRow && c + offsetCol == cursorCol) {
//                 terminal.printInvertedAt(rowPosition, colPosition + 1, cellText);
//             } else {
//                 terminal.printAt(rowPosition, colPosition + 1, " " + cellText + " ");
//             }
//         }
//     }

//     std::cout << "\033[0m" << std::flush;
// }
