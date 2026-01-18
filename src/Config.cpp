#include "Config.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_set>

namespace
{
// Domyslne wartosci konfiguracji.
constexpr int defaultWidth = 30;
constexpr int defaultHeight = 20;
constexpr int defaultTileSize = 24;
constexpr int defaultTickMs = 120;

void trim(std::string& text)
{
    // Proste przyciecie bialych znakow.
    const auto first = text.find_first_not_of(" \t\r\n");
    const auto last = text.find_last_not_of(" \t\r\n");

    if (first == std::string::npos)
    {
        text.clear();
        return;
    }

    text = text.substr(first, last - first + 1);
}

void validatePositive(const std::string& key, int value)
{
    // Wspolna walidacja liczb dodatnich.
    if (value <= 0)
    {
        throw std::invalid_argument("Config value for " + key + " must be positive");
    }
}
} // namespace

Config loadConfig(const std::filesystem::path& path)
{
    // Start od ustawien domyslnych.
    Config config{defaultWidth, defaultHeight, defaultTileSize, defaultTickMs};

    if (!std::filesystem::exists(path))
    {
        return config;
    }

    std::ifstream input(path);
    if (!input)
    {
        throw std::runtime_error("Failed to open config file: " + path.string());
    }

    std::unordered_set<std::string> seenKeys;
    std::string line;

    while (std::getline(input, line))
    {
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        // Oczekiwany format: klucz=wartosc.
        const auto eqPos = line.find('=');
        if (eqPos == std::string::npos)
        {
            throw std::invalid_argument("Invalid config line: " + line);
        }

        std::string key = line.substr(0, eqPos);
        std::string valueText = line.substr(eqPos + 1);
        trim(key);
        trim(valueText);

        int value = 0;
        try
        {
            value = std::stoi(valueText);
        }
        catch (const std::exception&)
        {
            throw std::invalid_argument("Invalid numeric value for key: " + key);
        }

        if (key == "width")
        {
            validatePositive(key, value);
            config.width = value;
        }
        else if (key == "height")
        {
            validatePositive(key, value);
            config.height = value;
        }
        else if (key == "tile_size")
        {
            validatePositive(key, value);
            config.tileSize = value;
        }
        else if (key == "tick_ms")
        {
            validatePositive(key, value);
            config.tickMs = value;
        }
        else
        {
            throw std::invalid_argument("Unknown config key: " + key);
        }

        seenKeys.insert(key);
    }

    const bool allKeysPresent = seenKeys.contains("width") && seenKeys.contains("height") &&
                                seenKeys.contains("tile_size") && seenKeys.contains("tick_ms");

    if (!allKeysPresent)
    {
        throw std::invalid_argument("Missing required config keys");
    }

    if (config.width < 3 || config.height < 3)
    {
        throw std::invalid_argument("Board size must be at least 3x3");
    }

    return config;
}
