#include "Spreadsheet.h"
#include "AnsiTerminal.h"
#include <iostream>
#include <iomanip>
#include <string>

#include <algorithm>
#include<fstream>

// Constructor: Initializes the spreadsheet with the specified number of rows and columns
Spreadsheet::Spreadsheet(int rows, int cols) : rows(rows), cols(cols) {
    //data.resize(rows, std::vector<std::string>(cols, "")); // Initialize all cells with empty strings
secondHeader =" ";
}

// The following function was implemented with assistance from ChatGPT.
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
void Spreadsheet::createNew(int newRows, int newCols) {
    data.clear(); // Önce mevcut verileri temizle
   // data.shrink_to_fit(); // Vektörün kapasitesini serbest bırak
    rows = newRows;
    cols = newCols;
   // data = std::vector<std::vector<std::string>>(rows, std::vector<std::string>(cols, ""));
    firsHeader.clear();
    secondHeader.clear();
}
void Spreadsheet::display(AnsiTerminal& terminal,int cursorRow, int cursorCol, int offsetRow,int offsetCol) {
    terminal.clearScreen();

    const int headerRow = 4;
    const int headerCol = 8;
    const int cellWidth = 10;

// Display selected cell info
    std::string cellLabel =  getColumnLabel(cursorCol) + std::to_string(cursorRow + 1);
    std::string cellContent = data.getValue(cursorRow + 1, cursorCol + 1); // Adjust to 1-based indexing
    std::string displayContent = cellContent.empty() ? " " : cellContent;
    terminal.printAt(1, 2, "\033[42m " + cellLabel + " (" + std::string(1, getContentType(displayContent)) + ") " + displayContent + " \033[0m");

    std::vector<std::string> operators = { "+", "-", "*", "/" };
    std::vector<std::string> formulaLabels = { "SUM", "@SUM", "AVER", "@AVER", "STDDEV", "@STDDEV", "MAX", "@MAX", "MIN", "@MIN" };
   
   // The following function was implemented with assistance from ChatGPT.
    std::unordered_map<RegexType, std::string> regexMap = {
    { RegexType::TokenPattern, "([A-Z][0-9]{1,3}|[\\+\\-\\*/]|(SUM|@SUM|STDDEV|@STDDEV|AVER|@AVER|MAX|@MAX|MIN|@MIN)\\(([A-Z]{1,2}[0-9]{1,3})\\.\\.([A-Z]{1,2}[0-9]{1,3})\\)|-?\\d*\\.?\\d+([eE][-+]?\\d+)?|\\w+)" },
    { RegexType::MatrixReference, "^[A-Z]{1,2}[0-9]{1,3}$" },
    { RegexType::Formula, "^(SUM|@SUM|STDDEV|@STDDEV|AVER|@AVER|MAX|@MAX|MIN|@MIN)\\(([A-Z]{1,2}[0-9]{1,3})\\.\\.([A-Z]{1,2}[0-9]{1,3})\\)$" },
    { RegexType::DecimalNumber, "^-?\\.\\d+$" },
    { RegexType::GeneralNumber, "^-?\\d*\\.?\\d+([eE][-+]?\\d+)?$" },
    { RegexType::AlphanumericLabel, ".*[A-Za-z].*[0-9].*|.*[0-9].*[A-Za-z].*" }
};

Tokenizer tokenizer(operators, formulaLabels, regexMap);

    LexicalAnalysis lexicalAnalyzer(tokenizer,data);




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

    for (int r = 0; r < windowSize && r + offsetRow < rows; ++r) {
        terminal.printAt(headerRow + r + 1, 2, "\033[42m " + std::to_string(r + offsetRow + 1) + " \033[0m");
    }

    for (int r = 0; r < windowSize && r + offsetRow < rows; ++r) {
        for (int c = 0; c < windowSize && c + offsetCol < cols; ++c) {
            int rowPosition = headerRow + r + 1;
            int colPosition = headerCol + c * cellWidth;

            // Get cell content
            std::string cellContent = data(r + offsetRow,c + offsetCol);
            std::string displayText=" ";
            if(cellContent!= "")
            {
                cellContent.resize(10, ' '); // Ensure fixed width for display

                // Analyze the current cell
                std::vector<Token> cellTokens = tokenizer.tokenize(data(r + offsetRow,c + offsetCol));
                // Check for empty tokens or unknown token types
                std::string analyzedValue;
                if (cellTokens.empty() || cellTokens[0].type == TokenType::Unknown) {
                    analyzedValue = cellContent.substr(0, 8); // Display raw content
                } else {
                    analyzedValue = lexicalAnalyzer.evaluateFormula(cellTokens);
                }

                // Determine display text based on analysis
                displayText = analyzedValue;
                if (analyzedValue.find("Error") != std::string::npos) {
                    displayText = cellContent; // Display "Err" for invalid formulas or references
                }
          
            if(lexicalAnalyzer.isNumeric(displayText))
            {
                displayText=lexicalAnalyzer.formatDecimal(displayText);
            }
            
                displayText.resize(10, ' '); // Ensure fixed width for display
            }
            else{
                cellContent.resize(10, ' '); // Ensure fixed width for display
                displayText=cellContent;
            }


            // Highlight the active cell
            if (r + offsetRow == cursorRow && c + offsetCol == cursorCol) {
                terminal.printInvertedAt(rowPosition, colPosition + 1, displayText);
            } else {
                terminal.printAt(rowPosition, colPosition + 1, " " + displayText + " ");
            }
        }
    }

    //terminal.printAt(150, 2, "TEst  rwr");    // 10x10 pencere içindeki sütun başlıklarını çiz
    std::cout << "\033[0m" << std::flush;
}

