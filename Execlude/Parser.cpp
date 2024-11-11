#include <cctype> // isdigit kullanımı için
#include <stdexcept> // istisnalar için
#include "Parser.h"

// Hücre referanslarını ve basit toplama işlemlerini çözümleyen temel bir değerlendirme fonksiyonu
int Parser::evaluate(const std::string& expression, const Spreadsheet& sheet) {
    if (expression.empty()) return 0;

    int result = 0;
    size_t pos = 0;
    bool add = true; // Toplama işlemi olup olmadığını takip eder

    while (pos < expression.length()) {
        if (expression[pos] == '=') {
            // Eğer ifade '=' ile başlıyorsa, formül demektir; bu durumda devam et
            pos++;
        }

        // Toplama ve çıkarma işaretlerini kontrol et
        if (expression[pos] == '+') {
            add = true;
            pos++;
        } else if (expression[pos] == '-') {
            add = false;
            pos++;
        }

        // Hücre referansını veya sayıyı çözümle
        int value = 0;
        if (isalpha(expression[pos])) {
            // Harf ile başlıyorsa bu bir hücre referansı (örneğin A1)
            size_t cellEnd = pos + 1;
            while (cellEnd < expression.length() && isalnum(expression[cellEnd])) cellEnd++;
            std::string cellRef = expression.substr(pos, cellEnd - pos);
            value = getCellReferenceValue(cellRef, sheet);
            pos = cellEnd;
        } else if (isdigit(expression[pos])) {
            // Rakam ile başlıyorsa doğrudan bir sayı (örneğin 10)
            size_t numEnd;
            value = std::stoi(expression.substr(pos), &numEnd);
            pos += numEnd;
        } else {
            throw std::invalid_argument("Invalid character in expression");
        }

        // Değeri toplama veya çıkarma işlemiyle sonucu güncelle
        result = add ? result + value : result - value;
    }

    return result;
}

// Hücre referansını çözümleyip, belirtilen hücrenin değerini döner
int Parser::getCellReferenceValue(const std::string& cellRef, const Spreadsheet& sheet) {
    if (cellRef.length() < 2) throw std::invalid_argument("Invalid cell reference");

    // Hücre referansını satır ve sütun indekslerine çevir
    int col = toupper(cellRef[0]) - 'A';
    int row = std::stoi(cellRef.substr(1)) - 1;

    // Hücre değerini al ve sayıya çevir
    std::string cellValue = sheet.getCellValue(row, col);
    if (cellValue.empty() || !isdigit(cellValue[0])) return 0; // Hücre boşsa 0 döner

    return std::stoi(cellValue); // Hücre değerini sayıya çevir
}
