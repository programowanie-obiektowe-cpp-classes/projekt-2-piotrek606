#pragma once

#include <filesystem>

// Konfiguracja wczytywana z data/config.txt.
struct Config
{
    int width{};
    int height{};
    int tileSize{};
    int tickMs{};
};

// Zwraca domyslne wartosci, gdy pliku brak, i waliduje gdy istnieje.
Config loadConfig(const std::filesystem::path& path);
