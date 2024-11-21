#include "AnsiTerminal.h"
#include "Spreadsheet.h"

#include <iostream>

void handleNavigation(char inputKey, int& cursorRow, int& cursorCol, int maxRows, int maxCols) {
    switch (inputKey) {
        case 'U': cursorRow = std::max(0, cursorRow - 1); break; // Yukarı
        case 'D': cursorRow = std::min(maxRows - 1, cursorRow + 1); break; // Aşağı
        case 'L': cursorCol = std::max(0, cursorCol - 1); break; // Sol
        case 'R': cursorCol = std::min(maxCols - 1, cursorCol + 1); break; // Sağ
        default: break;
    }
}
void updateOffsets(int& offsetRow, int& offsetCol, int cursorRow, int cursorCol, int windowSize) {
    // Aktif hücrenin pencerede kalmasını sağla
    if (cursorRow < offsetRow) offsetRow = cursorRow;  // Üst kenara ulaştı
    else if (cursorRow >= offsetRow + windowSize) offsetRow = cursorRow - windowSize + 1;  // Alt kenara ulaştı

    if (cursorCol < offsetCol) offsetCol = cursorCol;  // Sol kenara ulaştı
    else if (cursorCol >= offsetCol + windowSize) offsetCol = cursorCol - windowSize + 1;  // Sağ kenara ulaştı
}
void updateCellContent(Spreadsheet& sheet, int cursorRow, int cursorCol, char key, int& prevRow, int& prevCol) {
    std::string currentContent = sheet.getValue(cursorRow + 1, cursorCol + 1); // Mevcut içeriği al
    if (cursorRow == prevRow && cursorCol == prevCol) {
        // Aynı hücredeyiz, mevcut içeriğe karakter ekle
        currentContent += key; 
    } else {
        // Yeni hücreye geçildi, yeni karakteri başlat
        currentContent = key;
    }
    // Güncellenmiş içeriği ata ve ikinci başlığı güncelle
    sheet.setValue(cursorRow + 1, cursorCol + 1, currentContent);
    sheet.setSecondHeader(currentContent);
    prevRow = cursorRow;
    prevCol = cursorCol;
}

int main() {

    AnsiTerminal terminal;
    Spreadsheet sheet(20, 40); // 20 satır ve 8 sütunluk bir tablo oluşturun


    // terminal.printAt(50, 2, "\033[42m " + std::to_string(50 + 1) + " \033[0m");

    bool running = true;
    int cursorRow = 0, cursorCol = 0;
    int prevRow = cursorRow, prevCol = cursorCol; // Önceki seçili hücreyi takip et

    const int windowSize = 10;  // Görünen pencere boyutu
    int offsetRow = 0, offsetCol = 0;
    sheet.setWindowSize(10);
    bool editingMode=false;
    while (running) {
        updateOffsets(offsetRow, offsetCol, cursorRow, cursorCol, windowSize); // Ofseti güncelle
        sheet.display(terminal, cursorRow, cursorCol, offsetRow, offsetCol); // Tabloyu göster
        char inputKey = terminal.getSpecialKey(); // Kullanıcı girdisi al
        sheet.setSecondHeader(" "); // İkinci başlığı temizle

        if (inputKey == 'q') {
            running = false; // Çıkış
        } else if (strchr("UDLR", inputKey) && !editingMode) {
            // Hareket girdilerini işle
            handleNavigation(inputKey, cursorRow, cursorCol, sheet.getRows(), sheet.getCols());
        } else {
            // Diğer girdiler için hücre içeriğini güncelle
            if(inputKey=='\n')
            {
                editingMode= false;
                prevRow=-1;prevCol=-1;
                sheet.setSecondHeader("Editing Mode: Inactive");
            }
            else
            {
                //sheet.setSecondHeader("Editing Mode: Active");
                editingMode=true;
                updateCellContent(sheet, cursorRow, cursorCol, inputKey, prevRow, prevCol);
            }
        }
    }
    return 0;
}

