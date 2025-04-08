#include "ui.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/ioctl.h>

const std::string RESET   = "\033[0m";
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

// Count the number of visible (display) characters in a UTF-8 string.
// Only count UTF-8 lead bytes (i.e., ignore continuation bytes).
int countDisplayChars(const std::string& line) {
    int count = 0;
    for (unsigned char c : line) {
        if ((c & 0xC0) != 0x80) count++; // Count only UTF-8 lead bytes
    }
    return count;
}

int getFontMaxVisualWidth(const FontArray& font) {
    int maxWidth = 0;
    for (const auto& digit : font) {
        for (const auto& line : digit) {
            int w = countDisplayChars(line);
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