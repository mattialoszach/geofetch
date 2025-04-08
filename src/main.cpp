#include "fonts.hpp"
#include "ui.hpp"
#include "geo.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <tuple>
#include <string>
#include <algorithm>
#include <cctype>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>


void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    std::cout << "\033[?25h"; // Make Cursor visible
    std::cout << "\033[0m"; // Reset colors
    std::cout << "\033[2J\033[H"; // Clear screen
    std::cout.flush();
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON); // No Echo, no Line-Buffering
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    std::cout << "\033[?25l"; // Hide Cursor
}

// Non-blocking getchar
int kbhit() {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
}


// Print centered & stabilized Information (Location, Time, Date, Temperature)
void printInformation(const FontArray& font, std::string& location, std::string& dateAndTemp) {
    time_t now = time(0);
    struct tm *ltm = localtime(&now);

    int digits[8] = {
        ltm->tm_hour / 10, ltm->tm_hour % 10,
        10,
        ltm->tm_min / 10, ltm->tm_min % 10,
        10,
        ltm->tm_sec / 10, ltm->tm_sec % 10
    };

    int termWidth  = getTerminalWidth();
    int termHeight = getTerminalHeight();

    int charHeight = font[0].size();
    int charWidth  = getFontMaxVisualWidth(font);
    int clockWidth = 8 * (charWidth);  // 8 Character, Distance
    int clockHeight = charHeight + 2;

    int startCol = std::max(1, (termWidth - clockWidth) / 2); // ANSI min. Column 1
    int startRow = std::max(0, (termHeight - clockHeight) / 2);

    // Clear screem
    std::cout << "\033[2J\033[H";

    // Display Location
    printCentered(location, startRow, termWidth);

    // Display Time
    for (int row = 0; row < charHeight; ++row) {
        std::cout << "\033[" << (startRow + 2 + row) << ";" << startCol << "H";
        for (int i = 0; i < 8; ++i) {
            std::cout << font[digits[i]][row];
        }
    }

    // Display Date & Temp
    printCentered(dateAndTemp, startRow + charHeight + 3, termWidth);

    // Move cursor to the bottom of the terminal (for clean visual output)
    std::cout << "\033[" << termHeight << ";1H";

    std::cout.flush();
}

void showHelp() {
    std::cout << "\n";
    std::cout << "🌍🕒 geofetch - Terminal tool with Geo Info\n";
    std::cout << "------------------------------------------\n";
    std::cout << "Usage:\n";
    std::cout << "  geofetch               Run with default font and color\n";
    std::cout << "  geofetch -s <style>    Set font style (see 'style' command)\n";
    std::cout << "  geofetch -c <color>    Set font color (e.g. red, blue, cyan, ...)\n";
    std::cout << "  geofetch help          Show this help screen\n";
    std::cout << "  geofetch style         List available font styles\n";
    std::cout << "\n";
    std::cout << "Controls:\n";
    std::cout << "  Press 'q' or ESC to quit the clock\n";
    std::cout << "\n";
    std::cout << "\033[47;30mCreated by Mattia Loszach";
    std::cout << "\n";
}

void showStyles() {
    std::cout << "Available Font Styles:\n";
    for (auto& [name, _] : FONT_MAP) std::cout << " - " << name << "\n";
    std::cout << "\nUse one with: geofetch -s <style>\n";
}

// Parsing CLI arguments
bool parseArgs(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        // Show help if the argument is 'help'
        if (arg == "help") {
            showHelp();
            return false; // Stop execution after help message
        }

        if (arg == "style") {
            showStyles();
            return false; // Stop execution after style message
        }

        // When using '-c'
        if (arg == "-c") {
            if (i + 1 >= argc || argv[i + 1][0] == '-') {
                std::cerr << "Error: Missing color after '-c'.\n";
                std::cerr << "Usage: geofetch -c <color>\n";
                return false;
            }
            std::string opt = argv[i + 1];
            std::transform(opt.begin(), opt.end(), opt.begin(),
                   [](unsigned char c) { return std::tolower(c); });
            if (COLOR_MAP.find(opt) == COLOR_MAP.end()) {
                std::cerr << "Error: Unknown color '" << opt << "'.\n";
                std::cerr << "Available colors: red, green, yellow, blue, magenta, cyan, white\n";
                return false;
            }
            color = COLOR_MAP[opt];
        }
        
        // When using '-s'
        else if (arg == "-s") {
            if (i + 1 >= argc || argv[i + 1][0] == '-') {
                std::cerr << "Error: Missing style after '-s'.\n";
                std::cerr << "Usage: geofetch -s <style>\n";
                return false;
            }
            fontStyle = argv[i + 1];
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    // Parse CLI arguments and check for errors
    if (!parseArgs(argc, argv)) {
        return 1; // Exit on error
    }

    auto it = FONT_MAP.find(fontStyle);
    if (it == FONT_MAP.end()) {
        std::cerr << "Error: Font style '" << fontStyle << "' not found.\n";
        std::cerr << "Available font styles are:\n";
        for (auto& [name, _] : FONT_MAP) {
            std::cerr << "  - " << name << "\n";
        }
        std::cerr << "Use 'geofetch style' to list styles.\n";
        return 1;
    }

    const FontArray& selectedFont = *(it->second);
    FontArray& mutableFont = const_cast<FontArray&>(*it->second);
    int maxWidth = getFontMaxVisualWidth(mutableFont);
    padFontToWidth(mutableFont, maxWidth);

    auto [city, region, country] = getCityRegionCountry();
    std::string currentDate = getCurrentDate();
    std::string temperature = getTemperature(city);

    std::string location = "⚑ " + region + ", " + country;
    std::string dateAndTemp = currentDate + " | " + temperature;

    std::cout << color;

    enableRawMode();

    while (true) {
        printInformation(selectedFont, location, dateAndTemp);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (kbhit()) {
            char ch;
            read(STDIN_FILENO, &ch, 1);
            if (ch == 'q' || ch == 27) break; // 'q' or ESC
        }
    }

    disableRawMode();

    std::cout << RESET << std::endl; // Clean exit
    return 0;
}