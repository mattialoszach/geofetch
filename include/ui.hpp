#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <termios.h>

// ========== Farbdefinitionen ==========
extern const std::string RESET;
extern const std::string RED;
extern const std::string GREEN;
extern const std::string YELLOW;
extern const std::string BLUE;
extern const std::string MAGENTA;
extern const std::string CYAN;
extern const std::string WHITE;

extern std::unordered_map<std::string, std::string> COLOR_MAP;

// Globale Einstellungen
extern std::string color;
extern std::string fontStyle;

// Terminal-Konfiguration
extern struct termios orig_termios;

// ASCII-Font-Typ
using FontArray = std::vector<std::vector<std::string>>;

// ======= Funktionen (Deklarationen) =======
int getTerminalWidth();
int getTerminalHeight();
int countDisplayChars(const std::string& line);
int getFontMaxVisualWidth(const FontArray& font);
void printCentered(const std::string& text, int row, int termWidth);
void padFontToWidth(FontArray& font, int targetWidth);