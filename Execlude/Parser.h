#ifndef PARSER_H
#define PARSER_H

#include <string>
#include "Spreadsheet.h"

class Parser {
public:
    // Formülü çözümler ve sonucu döner
    static int evaluate(const std::string& expression, const Spreadsheet& sheet);

private:
    // Hücre referansını çözümleyerek değerini döner (örneğin A1, B2 gibi)
    static int getCellReferenceValue(const std::string& cellRef, const Spreadsheet& sheet);
};

#endif // PARSER1_H
