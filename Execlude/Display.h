#ifndef DISPLAY_H
#define DISPLAY_H

#include "Spreadsheet.h"

class Display {
public:
    // Constructor: Spreadsheet referansını alır
    Display(const Spreadsheet& sheet);

    // Tablonun içeriğini terminale yazdırır
    void show() const;
    void printHorizontalLine() const;
    void printColumnHeaders() const;
private:
    const Spreadsheet& sheet; // Spreadsheet referansı
    int cellWidth = 10;       // Hücre genişliği (her hücrenin kaplayacağı genişlik)
    
    // Yardımcı fonksiyonlar

};

#endif // DISPLAY_H
