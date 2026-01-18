#pragma once

#include "Board.hpp"
#include "Config.hpp"
#include "Food.hpp"
#include "Random.hpp"
#include "Snake.hpp"

#include <SFML/Graphics.hpp>
#include <filesystem>
#include <string>
#include <vector>

// Spina logike gry z wejscem, tickiem i renderem SFML.
class Game
{
public:
    Game(const Config& config, const std::filesystem::path& dataDir);

    void run();

private:
    // Wpis w tabeli wynikow.
    struct HighscoreEntry
    {
        std::string name;
        int score{};
    };

    // Stan gry i ekranu.
    enum class State
    {
        EnterName,
        Running,
        Paused,
        GameOver
    };

    void handleEvents();
    void update(float dt);
    // Jedna aktualizacja logiki w tiku.
    void processTick();
    void render();

    void reset();
    void spawnFood();

    void loadHighscores();
    void saveHighscores();
    // Aktualizuje wynik gracza i sortuje liste.
    void updateHighscores();
    // Laczy duplikaty i obcina do limitu.
    void normalizeHighscores(std::size_t limit);

    bool isOpposite(Direction next) const;
    void updateTexts();
    // Przetwarza wpisywanie nicku z klawiatury.
    void updateNameInput(char32_t unicode);

    Config config_;
    std::filesystem::path dataDir_;
    Board board_;
    Snake snake_;
    Food food_;
    Random random_;

    sf::RenderWindow window_;
    sf::Font font_;
    sf::Text scoreText_;
    sf::Text pauseText_;
    sf::Text promptText_;
    sf::Text nameText_;
    sf::Text gameOverText_;
    sf::Text instructionText_;
    sf::Text scoreboardText_;

    sf::RectangleShape snakeShape_;
    sf::RectangleShape foodShape_;

    float accumulator_{0.F};
    float tickSeconds_{0.F};
    int score_{0};
    std::vector<HighscoreEntry> highscores_;
    std::string playerName_;
    std::string nameInput_;
    bool highscoreRecorded_{false};
    State state_{State::EnterName};
    Direction pendingDirection_{Direction::Right};
};
