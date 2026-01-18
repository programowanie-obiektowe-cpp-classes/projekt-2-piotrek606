#include "Config.hpp"
#include "Game.hpp"

#include <cstdio>
#include <exception>
#include <filesystem>
#include <print>

int main()
{
    try
    {
        // Start gry i obsluga bledow konfiguracji.
        const std::filesystem::path dataDir = "data";
        const Config config = loadConfig(dataDir / "config.txt");

        Game game(config, dataDir);
        game.run();
    }
    catch (const std::exception& ex)
    {
        std::println(stderr, "Error: {}", ex.what());
        return 1;
    }
}
