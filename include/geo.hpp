#pragma once
#include <string>
#include <tuple>

// Liefert Stadt, Region, Land
std::tuple<std::string, std::string, std::string> getCityRegionCountry();

// Holt Temperatur von wttr.in für eine Stadt
std::string getTemperature(const std::string& city);

// Gibt aktuelles Datum als dd.mm.yyyy zurück
std::string getCurrentDate();