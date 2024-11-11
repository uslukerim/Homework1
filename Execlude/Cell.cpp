#include "Cell.h"

// Varsayılan yapıcı
Cell::Cell() : value("") {}

// Hücre değerini ayarlamak için
void Cell::setValue(const std::string& value) {
    this->value = value;
}

// Hücre değerini almak için
std::string Cell::getValue() const {
    return value;
}

// Hücrenin bir formül olup olmadığını kontrol eder
bool Cell::isFormula() const {
    // Basit bir formül tanımı: "=" karakteri ile başlıyorsa formüldür
    return !value.empty() && value[0] == '=';
}
