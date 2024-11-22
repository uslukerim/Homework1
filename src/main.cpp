#include "AnsiTerminal.h"
#include "Spreadsheet.h"

#include <iostream>
enum class ProgramMode {
    MainMenu,
    Spreadsheet
};
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
    std::string currentContent = sheet.data.getValue(cursorRow + 1, cursorCol + 1); // Mevcut içeriği al
    if (cursorRow == prevRow && cursorCol == prevCol) {
        // Aynı hücredeyiz, mevcut içeriğe karakter ekle
        currentContent += key; 
    } else {
        // Yeni hücreye geçildi, yeni karakteri başlat
        currentContent = key;
    }
    // Güncellenmiş içeriği ata ve ikinci başlığı güncelle
    sheet.data.setValue(cursorRow + 1, cursorCol + 1, currentContent);
    sheet.setSecondHeader(currentContent);
    prevRow = cursorRow;
    prevCol = cursorCol;
}
// Kullanıcıdan dosya adını alacak yardımcı fonksiyon
std::string getFileNameFromUser(AnsiTerminal& terminal, int windowSize, const std::string& promptMessage, int offsetX) {
    terminal.printInvertedAt(windowSize + 6, offsetX, promptMessage);
    std::string filename;
    char fileInputChar;
    int currentX = offsetX + promptMessage.length() + 2; // Başlangıç pozisyonu

    while ((fileInputChar = terminal.getSpecialKey()) != '\n') {
        filename += fileInputChar;
        terminal.printInvertedAt(windowSize + 6, currentX, filename);
    }
    return filename;
}

void handleMainMenu(Spreadsheet& sheet, AnsiTerminal& terminal, ProgramMode& mode, int windowSize, std::string& currentFile) {
    terminal.clearScreen();
    
    // Mevcut dosya adı bilgisi
    std::string currentFileDisplay = currentFile.empty() ? "Untitled" : currentFile;

    // Ana menü ve mevcut dosya adı
    std::string DownTabMenu = "1. Create New | 2. Select File | 3. Save File | 4. Save As | 5. Show Current File | q. Quit";
    terminal.printInvertedAt(windowSize + 6, 2, DownTabMenu);
    terminal.printInvertedAt(windowSize + 8, 2, "Current File: " + currentFileDisplay);

    // Dinamik olarak başlangıç X pozisyonu hesapla
    int offsetX = DownTabMenu.length() + 5;

    char inputKey = terminal.getSpecialKey(); // Kullanıcı girdisi al
    switch (inputKey) {
        case '1': { 
            sheet.createNew(20, 20); // Yeni bir tablo oluştur
            currentFile.clear();   // Yeni tablo için dosya adı temizlenir
            mode = ProgramMode::Spreadsheet; // Spreadsheet moduna geç
            break;
        }
        case '2': {
            // Dosya adını kullanıcıdan al
            currentFile = getFileNameFromUser(terminal, windowSize, "Enter file name to load: ", offsetX);
            if (sheet.data.loadFromFile(currentFile)) {
                terminal.printInvertedAt(windowSize + 6, offsetX, "File loaded successfully");
                mode = ProgramMode::Spreadsheet; // Spreadsheet moduna geç
            } else {
                terminal.printInvertedAt(windowSize + 6, offsetX, "Failed to load file.");
                currentFile.clear(); // Hatalı yükleme durumunda dosya adı temizlenir
            }
            break;
        }
        case '3': {
            // Save File
            if (currentFile.empty()) {
                terminal.printInvertedAt(windowSize + 6, offsetX, "No file name. Use 'Save As' instead.");
            } else if (sheet.data.saveToFile(currentFile)) {
                terminal.printInvertedAt(windowSize + 6, offsetX, "File saved successfully");
            } else {
                terminal.printInvertedAt(windowSize + 6, offsetX, "Failed to save file.");
            }
            break;
        }
        case '4': {
            // Save As
            std::string newFileName = getFileNameFromUser(terminal, windowSize, "Enter file name to save as: ", offsetX);
            if (sheet.data.saveToFile(newFileName)) {
                currentFile = newFileName; // Yeni dosya adı olarak güncelle
                terminal.printInvertedAt(windowSize + 6, offsetX, "File saved successfully");
            } else {
                terminal.printInvertedAt(windowSize + 6, offsetX, "Failed to save file.");
            }
            break;
        }
        case '5':
        {
            if(currentFile.empty())
            {
                terminal.printInvertedAt(windowSize + 6, offsetX, "File is not exist, you can Create New");
            }
            else
            {
                mode = ProgramMode::Spreadsheet; // Spreadsheet mod
            }
          break;
        }
        case 'q': {
            mode = ProgramMode::MainMenu;
            break;
        }
        default:
            break; // Geçersiz giriş, hiçbir şey yapma
    }
}

// Spread sheet işlemleri için bir fonksiyon
void handleSpreadsheet(Spreadsheet& sheet, AnsiTerminal& terminal, ProgramMode& mode, int& cursorRow, int& cursorCol, 
                       int& offsetRow, int& offsetCol, int windowSize, bool& editingMode, int& prevRow, int& prevCol) {
    updateOffsets(offsetRow, offsetCol, cursorRow, cursorCol, windowSize); // Ofseti güncelle
    sheet.display(terminal, cursorRow, cursorCol, offsetRow, offsetCol); // Spread sheet'i göster
    char inputKey = terminal.getSpecialKey(); // Kullanıcı girdisi al

    if (inputKey == 'q') {
        mode = ProgramMode::MainMenu; // Ana menüye dön
        prevRow = -1;
        prevCol = -1;
    } else if (strchr("UDLR", inputKey) && !editingMode) {
        handleNavigation(inputKey, cursorRow, cursorCol, sheet.getRows(), sheet.getCols());
    } else if (inputKey == '\n') {
        editingMode = false;
        prevRow = -1;
        prevCol = -1;
        sheet.setSecondHeader(editingMode ? "Editing Mode: Active" : "Editing Mode: Inactive");
    } else {
        editingMode = true;
        updateCellContent(sheet, cursorRow, cursorCol, inputKey, prevRow, prevCol);
    }
}

int main() {
    AnsiTerminal terminal;
    Spreadsheet sheet(20, 40); // 20 satır ve 8 sütunluk bir tablo oluşturun
    ProgramMode mode = ProgramMode::MainMenu; // Başlangıç modu menü


    // terminal.printAt(50, 2, "\033[42m " + std::to_string(50 + 1) + " \033[0m");

    bool running = true;
    int cursorRow = 0, cursorCol = 0;
    int prevRow = cursorRow, prevCol = cursorCol; // Önceki seçili hücreyi takip et

    const int windowSize = 10;  // Görünen pencere boyutu
    int offsetRow = 0, offsetCol = 0;
    sheet.setWindowSize(10);
    bool editingMode=false;
    // std::string DownTabMenu ="Create New(1)  Select File(2) Save Current File(3) 4.Show Current File Quit(q)";//std::string DownTabMenu[4]={ "Create New(1)","Select File(2)"," Save Current File(3)", "Quit(q)"};
    // std::string DownTabMenuSelection="   ";
    char fileInputChar='_';
    std::string filename="";
  while (running) {
        switch (mode) {
            case ProgramMode::MainMenu:
                handleMainMenu(sheet, terminal, mode, windowSize, filename);
                break;
            case ProgramMode::Spreadsheet:
                handleSpreadsheet(sheet, terminal, mode, cursorRow, cursorCol, offsetRow, offsetCol, 
                                  windowSize, editingMode, prevRow, prevCol);
                break;
        }
    }
    return 0;
}

