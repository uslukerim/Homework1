/**
 * @author kuslu
 */

#include "CellMatrix.h"

/**
 * @brief Constructs a CellMatrix with the specified number of rows and columns.
 * @param rows Initial number of rows.
 * @param cols Initial number of columns.
 */
CellMatrix::CellMatrix(int rows, int cols)
    : rows(rows), cols(cols), data(rows, std::vector<std::string>(cols, "")),
    lastRow(-1), lastCol(-1) {
    if (rows > MAXROWSIZE || cols > MAXCOLUMNSIZE) {
        throw std::out_of_range("Initial size exceeds maximum allowed dimensions.");
    }
}
void CellMatrix::resizeIfNeeded(int newRow, int newCol) {
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
 * @brief Accesses a cell value without modification.
 * @param row The row index of the cell.
 * @param col The column index of the cell.
 * @return A constant reference to the string value at the specified cell.
 */
const std::string& CellMatrix::operator()(int row, int col) const {
    static const std::string empty = "";
    if (!isValid(row, col)) {
        return empty;
    }
    
    return data[row][col]; // Retrieve value using 0-based indexing
}

 /**
 * @brief Retrieves the value of a cell in the matrix at the specified row and column.
 * 
 * */
const std::string CellMatrix::getValue(int row, int col) const 
{
    if (row >= 1 && row <= rows && col >= 1 && col <= cols) {
        return data[row - 1][col - 1]; // Retrieve value using 0-based indexing
    }
    return ""; // Return an empty string if out of bounds
}
/**
 * @brief Sets the content of a specific cell.
 */
void CellMatrix::setValue(int row, int col, const std::string& value) {
    if (row >= 1 && col >= 1) { // Ensure valid 1-based indexing
        resizeIfNeeded(row, col); // Resize if necessary

        // Remove newline characters from the input value
        std::string filteredValue = value;
        filteredValue.erase(std::remove(filteredValue.begin(), filteredValue.end(), '\n'), filteredValue.end());

        // Assign the filtered value to the cell
        data[row - 1][col - 1] = filteredValue; // 0-based indexing internally
    }
}
/**
 * @brief Resizes the matrix to the specified dimensions.
 * @param newRows The new number of rows.
 * @param newCols The new number of columns.
 */
void CellMatrix::resize(int newRows, int newCols) {
    if (newRows > MAXROWSIZE || newCols > MAXCOLUMNSIZE) {
        throw std::out_of_range("Resize exceeds maximum allowed dimensions.");
    }
    data.resize(newRows);
    for (auto& row : data) {
        row.resize(newCols, "");
    }
    rows = newRows;
    cols = newCols;
}
/**
 * @brief Gets the current number of rows in the matrix.
 * @return The number of rows.
 */
int CellMatrix::getRows() const {
    return rows;
}

/**
 * @brief Gets the current number of columns in the matrix.
 * @return The number of columns.
 */
int CellMatrix::getCols() const {
    return cols;
}

/**
 * @brief Checks if the given cell index is within the valid range.
 * @param row The row index to check.
 * @param col The column index to check.
 * @return True if the indices are valid, false otherwise.
 */
bool CellMatrix::isValid(int row, int col) const {
    return row >= 0 && row < rows && col >= 0 && col < cols;
}

/**
 * @brief Resizes the matrix to fit the specified indices.
 * @param newRows The minimum number of rows required.
 * @param newCols The minimum number of columns required.
 */
void CellMatrix::resizeToFit(int newRows, int newCols) {
    if (newRows > MAXROWSIZE || newCols > MAXCOLUMNSIZE) {
        throw std::out_of_range("Resize exceeds maximum allowed dimensions.");
    }
    resize(std::max(newRows, rows), std::max(newCols, cols));
}
///@brief: Clears the contents of all cells in the matrix.
void CellMatrix::clear() {
    for (auto& row : data) {
        for (auto& cell : row) {
            cell.clear();
        }
    }
    resize(1, 1); // Reset the matrix to 1x1 size

}

/**
 * @brief Loads the matrix data from a CSV file.
 * @return True if the file was loaded successfully, false otherwise.
 */
bool CellMatrix::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file for reading: " << filename << "\n";
        return false;
    }

    std::vector<std::vector<std::string>> tempData;
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> row;

        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        tempData.push_back(row);
    }

    file.close();

    // Update the matrix dimensions and data
    rows = tempData.size();
    cols = rows > 0 ? tempData[0].size() : 0;
    data = std::move(tempData);

    return true;
}

/**
 * @brief Saves the matrix data to a CSV file.
 * @return True if the file was saved successfully, false otherwise.
 */
bool CellMatrix::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file for writing: " << filename << "\n";
        return false;
    }

    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << row[i];
            if (i < row.size() - 1) {
                file << ",";
            }
        }
        file << "\n";
    }

    file.close();
    return true;
}