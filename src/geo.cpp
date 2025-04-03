#include "geo.hpp"
#include "ui.hpp"
#include <iostream>
#include <tuple>
#include <string>
#include <cstdio>
#include <ctime>
#include <sstream>
#include <iomanip>

std::tuple<std::string, std::string, std::string> getCityRegionCountry() {
    std::string city, region, country;
    char buffer[256];

    FILE* pipe = popen("curl -s ipinfo.io", "r");
    if (!pipe) return {"Unknown", "Unknown", "Unknown"};

    while (fgets(buffer, sizeof buffer, pipe)) {
        std::string line(buffer);
        if (line.find("\"city\"") != std::string::npos) {
            size_t start = line.find(": \"") + 3;
            size_t end = line.find("\",", start);
            city = line.substr(start, end - start);
        } else if (line.find("\"region\"") != std::string::npos) {
            size_t start = line.find(": \"") + 3;
            size_t end = line.find("\",", start);
            region = line.substr(start, end - start);
        } else if (line.find("\"country\"") != std::string::npos) {
            size_t start = line.find(": \"") + 3;
            size_t end = line.find("\",", start);
            country = line.substr(start, end - start);
        }
    }

    pclose(pipe);
    return {
        city.empty() ? "Unknown" : city,
        region.empty() ? "Unknown" : region,
        country.empty() ? "Unknown" : country
    };
}

std::string getTemperature(const std::string& city) {
    std::string cmd = "curl -s \"wttr.in/" + city + "?format=%t\"";
    std::string result;
    char buffer[128];
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "N/A";

    while (fgets(buffer, sizeof buffer, pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);

    if (!result.empty() && result.back() == '\n')
        result.pop_back();

    return result;
}

std::string getCurrentDate() {
    std::time_t t = std::time(nullptr);
    std::tm *now = std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(now, "%d.%m.%Y");
    return oss.str();
}