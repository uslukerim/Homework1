/**
 * @file Spreadsheet.h
 * @brief Declaration of the Spreadsheet class for managing a terminal-based spreadsheet application.
 *
 * The Spreadsheet class provides functionality for creating, displaying, and managing
 * a spreadsheet using ANSI terminal features. It includes operations for setting headers,
 * determining content types, managing a scrollable window view, and handling the underlying
 * cell matrix data.
 */

#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include <vector>
#include <string>
#include "AnsiTerminal.h" // Include for terminal display functionalities
#include "Tokenizer.h"
#include "LexicalAnalysis.h"
#include "CellMatrix.h"

/**
 * @brief Represents a spreadsheet for managing and displaying data.
 *
 * The Spreadsheet class provides methods to set headers, display the spreadsheet,
 * determine content types, and manage rows, columns, and cell data. It also supports
 * resizing the visible window of the spreadsheet and creating new spreadsheets.
 */
class Spreadsheet {
public:
    /**
     * @brief Constructs a Spreadsheet with the specified dimensions.
     * 
     * @param rows The number of rows in the spreadsheet.
     * @param cols The number of columns in the spreadsheet.
     */
    Spreadsheet(int rows, int cols);

    /**
     * @brief Sets the first header of the spreadsheet.
     * 
     * @param row The row index for the header.
     * @param col The column index for the header.
     * @param value The header value to set.
     */
    void setFirstHeader(int row, int col, const std::string& value);

    /**
     * @brief Sets the second header of the spreadsheet.
     * 
     * @param value The header value to set.
     */
    void setSecondHeader(const std::string& value);

    /**
     * @brief Displays the spreadsheet in the terminal.
     * 
     * @param terminal The AnsiTerminal object for display operations.
     * @param cursorRow The current row of the cursor.
     * @param cursorCol The current column of the cursor.
     * @param offsetRow The row offset for scrolling.
     * @param offsetCol The column offset for scrolling.
     */
    void display(AnsiTerminal& terminal, int cursorRow, int cursorCol, int offsetRow, int offsetCol);

    /**
     * @brief Determines the content type of a given string.
     * 
     * @param content The content to evaluate.
     * @return 'L' if the content is a label (string), 'V' if it is a value (number).
     */
    char getContentType(const std::string& content) const;

    /**
     * @brief Gets the number of rows in the spreadsheet.
     * 
     * @return The number of rows.
     */
    int getRows() const { return rows; }

    /**
     * @brief Gets the number of columns in the spreadsheet.
     * 
     * @return The number of columns.
     */
    int getCols() const { return cols; }

    /**
     * @brief Gets the label for a given column index.
     * 
     * @param index The column index.
     * @return The column label as a string.
     */
    std::string getColumnLabel(int index) const;

    /**
     * @brief Gets the current window size for the visible portion of the spreadsheet.
     * 
     * @return The window size.
     */
    int getWindowSize() const { return windowSize; }

    /**
     * @brief Sets the window size for the visible portion of the spreadsheet.
     * 
     * @param size The new window size.
     */
    void setWindowSize(int size) { windowSize = size; }

    /**
     * @brief Creates a new spreadsheet with the specified dimensions.
     * 
     * @param newRows The number of rows for the new spreadsheet.
     * @param newCols The number of columns for the new spreadsheet.
     */
    void createNew(int newRows, int newCols);

    /**
     * @brief Manages the cell data for the spreadsheet.
     */
    CellMatrix data;

private:
    int rows; ///< Number of rows in the spreadsheet.
    int cols; ///< Number of columns in the spreadsheet.
    std::string firsHeader; ///< The first header of the spreadsheet.
    std::string secondHeader; ///< The second header of the spreadsheet.
    int windowSize; ///< The size of the visible window in the spreadsheet.
};

#endif // SPREADSHEET_H
