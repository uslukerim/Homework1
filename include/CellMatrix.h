/**
 * @author kuslu
 */

#ifndef CELL_MATRIX_H
#define CELL_MATRIX_H

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
/**
 * @file CellMatrix.h
 * @brief Defines the CellMatrix class for managing a 2D grid of cells.
 */

/**
 * @brief Maximum number of rows allowed in the matrix.
 */
#define MAXROWSIZE 100

/**
 * @brief Maximum number of columns allowed in the matrix.
 */
#define MAXCOLUMNSIZE 100

/**
 * @class CellMatrix
 * @brief Represents a 2D matrix of cells, each storing a string value.
 * 
 * This class provides dynamic resizing capabilities while enforcing a maximum 
 * row and column limit. It supports direct access to cell values via 
 * operator() overloading.
 */
class CellMatrix {
public:
    /**
     * @brief Constructs a CellMatrix with the specified number of rows and columns.
     * @param rows Initial number of rows.
     * @param cols Initial number of columns.
     * @throws std::out_of_range If rows or cols exceed the defined maximum sizes.
     */
    CellMatrix(int rows=20, int cols=20);


    void resizeIfNeeded(int newRow, int newCol);
    // /**
    //  * @brief Accesses a cell value for modification.
    //  * @param row The row index of the cell.
    //  * @param col The column index of the cell.
    //  * @return A reference to the string value at the specified cell.
    //  * @throws std::out_of_range If resizing exceeds the maximum limits.
    //  */
    // std::string& operator()(int row, int col);

    /**
     * @brief Accesses a cell value without modification.
     * @param row The row index of the cell.
     * @param col The column index of the cell.
     * @return A constant reference to the string value at the specified cell. 
     * Returns an empty string if out of range.
     */
    const std::string& operator()(int row, int col) const;
    
    const std::string getValue(int row, int col) const ;

    /**
     * @brief Assigns a value to a cell.
     * @param value The string value to assign to the cell.
     * @return A reference to the current object (for chaining).
     */
    CellMatrix& operator=(const std::string& value);



    /**
     * @brief Sets the content of a specific cell.
     * @param row The row index of the cell.
     * @param col The column index of the cell.
     * @param value The new content for the cell.
     */
    void setValue(int row, int col, const std::string& value);


    /**
     * @brief Resizes the matrix to the specified dimensions.
     * @param newRows The new number of rows.
     * @param newCols The new number of columns.
     * @throws std::out_of_range If new dimensions exceed the maximum limits.
     */
    void resize(int newRows, int newCols);

    /**
     * @brief Gets the current number of rows in the matrix.
     * @return The number of rows.
     */
    int getRows() const;

    /**
     * @brief Gets the current number of columns in the matrix.
     * @return The number of columns.
     */
    int getCols() const;

    void clear();



    /**
     * @brief Loads the matrix data from a CSV file.
     * @param filename The path to the CSV file.
     * @return True if the file was loaded successfully, false otherwise.
     */
    bool loadFromFile(const std::string& filename);

    /**
     * @brief Saves the matrix data to a CSV file.
     * @param filename The path to the CSV file.
     * @return True if the file was saved successfully, false otherwise.
     */
    bool saveToFile(const std::string& filename) const;

private:
    int rows;  ///< Current number of rows in the matrix.
    int cols;  ///< Current number of columns in the matrix.
    std::vector<std::vector<std::string>> data; ///< 2D vector storing cell values.

    /**
     * @brief Checks if the given cell index is within the valid range.
     * @param row The row index to check.
     * @param col The column index to check.
     * @return True if the indices are valid, false otherwise.
     */
    bool isValid(int row, int col) const;

    /**
     * @brief Resizes the matrix to fit the specified indices.
     * @param newRows The minimum number of rows required.
     * @param newCols The minimum number of columns required.
     * @throws std::out_of_range If resizing exceeds the maximum limits.
     */
    void resizeToFit(int newRows, int newCols);

        // Used for tracking the last accessed cell for assignment
    int lastRow; ///< Last accessed row index.
    int lastCol; ///< Last accessed column index.
};

#endif // CELL_MATRIX_H
