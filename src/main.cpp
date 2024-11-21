#include "AnsiTerminal.h"
#include "Spreadsheet.h"

#include <iostream>

int main() {


    //     // Define operators and formula labels
    // std::vector<std::string> operators = { "+", "-", "*", "/" };
    // std::vector<std::string> formulaLabels = { "SUM", "AVER", "MAX", "MIN" };

    // // Initialize a 2D data matrix (representing cells in a spreadsheet)
    // std::vector<std::vector<int>> data = {
    //     {1, 2, 3, 4},
    //     {5, 6, 7, 8},
    //     {9, 10, 11, 12},
    //     {13, 14, 15, 16}
    // };
    //     data[0].resize(50);
    // // Create a Tokenizer and LexicalAnalysis object
    // Tokenizer tokenizer(operators, formulaLabels);
    // LexicalAnalysis lexicalAnalysis(tokenizer, data);

    // // Test expressions
    // std::vector<std::string> expressions = {
  
    //     "AA1+5+AVER(A1..A3)",
    // };

    // // Analyze each expression
    // for (const auto& expression : expressions) {
    //     std::cout << "Analyzing expression: " << expression << std::endl;
    //     lexicalAnalysis.analyze(expression);
    //     std::cout << "-----------------------------" << std::endl;
    // }
/**************************************************************************************/

    AnsiTerminal terminal;
    Spreadsheet sheet(20, 40); // 20 satır ve 8 sütunluk bir tablo oluşturun


    // terminal.printAt(50, 2, "\033[42m " + std::to_string(50 + 1) + " \033[0m");

    bool running = true;
    int cursorRow = 0, cursorCol = 0;
    int prevRow = cursorRow, prevCol = cursorCol; // Önceki seçili hücreyi takip et

    const int windowSize = 10;  // Görünen pencere boyutu
    int offsetRow = 0, offsetCol = 0;
    sheet.setWindowSize(10);
    //terminal.printInvertedAt(5, 10, "Merhaba, Dünya!");
while (running) {
    // Aktif hücrenin pencerede kalmasını sağla
    if (cursorRow < offsetRow) offsetRow = cursorRow;  // Üst kenara ulaştı
    else if (cursorRow >= offsetRow + windowSize) offsetRow = cursorRow - windowSize + 1;  // Alt kenara ulaştı

    if (cursorCol < offsetCol) offsetCol = cursorCol;  // Sol kenara ulaştı
    else if (cursorCol >= offsetCol + windowSize) offsetCol = cursorCol - windowSize + 1;  // Sağ kenara ulaştı

    // Tüm tabloyu göster, sadece 10x10 pencereyi güncel offset değerleriyle göster
    sheet.display(terminal, cursorRow, cursorCol, offsetRow, offsetCol);
    // Kullanıcının girdisini al
    char key = terminal.getSpecialKey();
    sheet.setSecondHeader(" ");             // İkinci başlığı temizle


    // Kullanıcı girdisine göre hareket et
    switch (key) {
        case 'U':
            cursorRow = std::max(0, cursorRow - 1); // Yukarı hareket
            break;

        case 'D':
            cursorRow = std::min(sheet.getRows() - 1, cursorRow + 1); // Aşağı hareket
            break;

        case 'L':
            cursorCol = std::max(0, cursorCol - 1); // Sol hareket
            break;

        case 'R':
            cursorCol = std::min(sheet.getCols() - 1, cursorCol + 1); // Sağ hareket
            break;

        case 'q':
            running = false; // Çıkış
            break;

        default:
            // Aynı hücredeyiz, mevcut içeriğe karakter ekle
            if (cursorRow == prevRow && cursorCol == prevCol) {
                std::string currentContent = sheet.getValue(cursorRow + 1, cursorCol + 1); // Mevcut içeriği al
                currentContent += key; // Yeni karakteri ekle
                sheet.setValue(cursorRow + 1, cursorCol + 1, currentContent); // Güncellenmiş içeriği hücreye ata
                sheet.setSecondHeader(currentContent); // İkinci başlık olarak hücre içeriğini göster
            } else {
                // Yeni hücreye geçildi, yeni karakteri atar ve header’ı günceller
                std::string newContent(1, key); // Yeni içerik tek karakter olarak oluşturulur
                sheet.setValue(cursorRow + 1, cursorCol + 1, newContent);
                sheet.setSecondHeader(newContent); // İkinci başlığı yeni içerik ile günceller
            }

            // `prevRow` ve `prevCol` değerlerini güncel hücreye ayarlayın
            prevRow = cursorRow;
            prevCol = cursorCol;
            break;
    }
}

    return 0;
}

