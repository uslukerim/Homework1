#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include <vector>
#include <string>
#include "AnsiTerminal.h" // AnsiTerminal sınıfını tanıtmak için ekleyin
#include "Tokenizer.h"
#include "LexicalAnalysis.h"
class Spreadsheet {
public:
    Spreadsheet(int rows, int cols);
    void setValue(int row, int col, const std::string& value);
    void setFirstHeader(int row, int col, const std::string& value);
    void setSecondHeader(const std::string& value);

    std::string getValue(int row, int col) const;
    void display(AnsiTerminal& terminal,int cursorRow, int cursorCol, int offsetRow,int offsetCol);
    char getContentType(const std::string& content)const;
    int getRows() const { return rows; }
    int getCols() const { return cols; }
    std::string getColumnLabel(int index) const;
    int getWindowSize() const { return windowSize; }
    void setWindowSize(int size) { windowSize = size; }
private:
    void resizeIfNeeded(int newRow, int newCol);
    int rows, cols;
    std::string firsHeader,secondHeader;
    std::vector<std::vector<std::string>> data;
    int windowSize;
};

#endif // SPREADSHEET_H
