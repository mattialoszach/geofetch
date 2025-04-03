#include "ui.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/ioctl.h>

const std::string RESET   = "\033[0m";  // Reset zur Standardfarbe
const std::string RED     = "\033[31m";
const std::string GREEN   = "\033[32m";
const std::string YELLOW  = "\033[33m";
const std::string BLUE    = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN    = "\033[36m";
const std::string WHITE   = "\033[37m";

std::unordered_map<std::string, std::string> COLOR_MAP = {
    {"red", RED}, {"green", GREEN}, {"yellow", YELLOW},
    {"blue", BLUE}, {"magenta", MAGENTA}, {"cyan", CYAN},
    {"white", WHITE}
};

std::string color = RESET;
std::string fontStyle = "block";

struct termios orig_termios;

// Terminalgröße abfragen
int getTerminalWidth() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}

int getTerminalHeight() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_row;
}

// UTF-8: Zeichenbreite korrekt zählen (sichtbare Zeichen)
int countDisplayChars(const std::string& line) {
    int count = 0;
    for (unsigned char c : line) {
        if ((c & 0xC0) != 0x80) count++; // Nur UTF-8 Lead-Bytes zählen
    }
    return count;
}

int getFontMaxVisualWidth(const FontArray& font) {
    int maxWidth = 0;
    for (int d = 0; d < 11; ++d) {
        for (int row = 0; row < 7; ++row) {
            int w = countDisplayChars(font[d][row]);
            if (w > maxWidth) maxWidth = w;
        }
    }
    return maxWidth;
}

void printCentered(const std::string& text, int row, int termWidth) {
    int col = std::max(1, (termWidth - countDisplayChars(text)) / 2);
    std::cout << "\033[" << row << ";" << col << "H" << text;
}

void padFontToWidth(FontArray& font, int targetWidth) {
    for (int d = 0; d < 11; ++d) {
        for (auto& line : font[d]) {
            int w = countDisplayChars(line);
            if (w < targetWidth) {
                line += std::string(targetWidth - w, ' ');
            }
        }
    }
}