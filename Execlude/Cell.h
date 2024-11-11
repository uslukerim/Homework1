#ifndef CELL_H
#define CELL_H

#include <string>

class Cell {
public:
    // Hücreyi bir değerle başlatır
    Cell();

    // Hücre değerini ayarlamak için bir fonksiyon
    void setValue(const std::string& value);

    // Hücre değerini almak için bir fonksiyon
    std::string getValue() const;

    // Hücre bir formül içeriyorsa true döndürür
    bool isFormula() const;

private:
    std::string value; // Hücrenin içeriği (sayı, metin veya formül)
};

#endif // CELL_H
