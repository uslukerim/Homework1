#include "Display.h"
#include <iostream>
#include <iomanip> // setw için

// Yapıcı: Spreadsheet referansını kaydeder
Display::Display(const Spreadsheet& sheet) : sheet(sheet) {}

// Tablonun içeriğini gösterir
void Display::show() const {
    printColumnHeaders();
    printHorizontalLine();

    for (int row = 0; row < sheet.getRowCount(); ++row) {
        // Satır numarasını yazdır
        std::cout << std::setw(3) << (row + 1) << " | ";
        
        // Hücre değerlerini yazdır
        for (int col = 0; col < sheet.getColumnCount(); ++col) {
            std::cout << std::setw(cellWidth) << sheet.getCellValue(row, col) << " | ";
        }
        
        std::cout << "\n";
        printHorizontalLine();
    }
}

// Yatay ayırıcı çizgi çizme
void Display::printHorizontalLine() const {
    std::cout << "----";
    for (int col = 0; col < sheet.getColumnCount(); ++col) {
        std::cout << std::string(cellWidth + 3, '-');
    }
    std::cout << "\n";
}

// Sütun başlıklarını yazdırır (A, B, C, ...)
void Display::printColumnHeaders() const {
    std::cout << "    "; // İlk hücreyi satır numaraları için boş bırak
    for (int col = 0; col < sheet.getColumnCount(); ++col) {
        std::cout << std::setw(cellWidth) << char('A' + col) << " | ";
    }
    std::cout << "\n";
}
