#pragma once
#include <string>
#include <tuple>

// Outputs City, Region, Country
std::tuple<std::string, std::string, std::string> getCityRegionCountry();

// Outputs Temperature using wttr.in for a city
std::string getTemperature(const std::string& city);

// Outputs current Date as dd.mm.yyyy (European System)
std::string getCurrentDate();