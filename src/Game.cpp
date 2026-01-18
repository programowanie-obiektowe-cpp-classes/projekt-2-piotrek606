#include "Game.hpp"

#include <SFML/Window/Event.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>

namespace
{
// Ustawienia startowe gry.
constexpr int initialLength = 3;
const std::string fontFile = "JetBrainsMono-Regular.ttf";
const std::string highscoreFile = "highscore.txt";
constexpr std::size_t maxNameLength = 12;

// Mapowanie klawiszy na kierunek.
Direction directionFromKey(sf::Keyboard::Key key)
{
    switch (key)
    {
    case sf::Keyboard::Key::Up:
    case sf::Keyboard::Key::W:
        return Direction::Up;
    case sf::Keyboard::Key::Down:
    case sf::Keyboard::Key::S:
        return Direction::Down;
    case sf::Keyboard::Key::Left:
    case sf::Keyboard::Key::A:
        return Direction::Left;
    case sf::Keyboard::Key::Right:
    case sf::Keyboard::Key::D:
    default:
        return Direction::Right;
    }
}

void centerText(sf::Text& text, const sf::Vector2f& center)
{
    // Ustawia srodek tekstu w zadanym punkcie.
    const sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.position + bounds.size / 2.F);
    text.setPosition(center);
}
} // namespace

Game::Game(const Config& config, const std::filesystem::path& dataDir)
    : config_(config),
      dataDir_(dataDir),
      board_(config.width, config.height),
      snake_({std::clamp(config.width / 2, initialLength - 1, config.width - 1),
              config.height / 2},
             initialLength, Direction::Right),
      scoreText_(font_, "", static_cast<unsigned int>(config.tileSize)),
      pauseText_(font_, "", static_cast<unsigned int>(config.tileSize + 6)),
      promptText_(font_, "", static_cast<unsigned int>(config.tileSize + 6)),
      nameText_(font_, "", static_cast<unsigned int>(config.tileSize + 6)),
      gameOverText_(font_, "", static_cast<unsigned int>(config.tileSize * 2)),
      instructionText_(font_, "", static_cast<unsigned int>(config.tileSize)),
      scoreboardText_(font_, "", static_cast<unsigned int>(config.tileSize)),
      window_(sf::VideoMode(
                  {static_cast<unsigned int>(config.width * config.tileSize),
                   static_cast<unsigned int>(config.height * config.tileSize)}),
              "Snake")
{
    window_.setFramerateLimit(60);
    tickSeconds_ = static_cast<float>(config_.tickMs) / 1000.F;

    snakeShape_.setSize(sf::Vector2f(static_cast<float>(config_.tileSize),
                                     static_cast<float>(config_.tileSize)));
    snakeShape_.setFillColor(sf::Color(30, 160, 60));

    foodShape_.setSize(sf::Vector2f(static_cast<float>(config_.tileSize),
                                    static_cast<float>(config_.tileSize)));
    foodShape_.setFillColor(sf::Color(220, 80, 60));

    const auto fontPath = dataDir_ / fontFile;
    if (!font_.openFromFile(fontPath))
    {
        throw std::runtime_error("Failed to load font: " + fontPath.string());
    }

    scoreText_.setFillColor(sf::Color::White);
    scoreText_.setPosition({4.F, 2.F});

    pauseText_.setFillColor(sf::Color(250, 220, 70));
    promptText_.setFillColor(sf::Color::White);
    nameText_.setFillColor(sf::Color(120, 220, 120));
    gameOverText_.setFillColor(sf::Color(240, 80, 60));
    instructionText_.setFillColor(sf::Color(220, 220, 220));
    scoreboardText_.setFillColor(sf::Color::White);

    loadHighscores();
    spawnFood();
    updateTexts();
}

void Game::run()
{
    sf::Clock clock;

    while (window_.isOpen())
    {
        handleEvents();
        const float dt = clock.restart().asSeconds();
        update(dt);
        render();
    }
}

void Game::handleEvents()
{
    while (const std::optional<sf::Event> event = window_.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window_.close();
        }
        else if (state_ == State::EnterName)
        {
            // W tym stanie obslugujemy wpisywanie nicku.
            if (const auto* text = event->getIf<sf::Event::TextEntered>())
            {
                updateNameInput(text->unicode);
                updateTexts();
            }
            else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                {
                    window_.close();
                }
                else if (key->code == sf::Keyboard::Key::Backspace)
                {
                    if (!nameInput_.empty())
                    {
                        nameInput_.pop_back();
                        updateTexts();
                    }
                }
                else if (key->code == sf::Keyboard::Key::Enter)
                {
                    if (nameInput_.empty())
                    {
                        nameInput_ = "PLAYER";
                    }
                    playerName_ = nameInput_;
                    state_ = State::Running;
                    updateTexts();
                }
            }
        }
        else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
        {
            if (key->code == sf::Keyboard::Key::Escape)
            {
                window_.close();
            }
            else if (key->code == sf::Keyboard::Key::P && state_ != State::GameOver)
            {
                state_ = state_ == State::Paused ? State::Running : State::Paused;
                if (state_ == State::Paused)
                {
                    accumulator_ = 0.F;
                }
                updateTexts();
            }
            else if (key->code == sf::Keyboard::Key::R)
            {
                reset();
            }
            else
            {
                const Direction requested = directionFromKey(key->code);
                if (!isOpposite(requested))
                {
                    pendingDirection_ = requested;
                }
            }
        }
    }
}

void Game::update(float dt)
{
    if (state_ != State::Running)
    {
        return;
    }

    // Akumulator dla stalych tickow.
    accumulator_ += dt;

    while (accumulator_ >= tickSeconds_)
    {
        processTick();
        accumulator_ -= tickSeconds_;

        if (state_ != State::Running)
        {
            break;
        }
    }
}

void Game::processTick()
{
    snake_.setDirection(pendingDirection_);
    const GridPos nextHead = snake_.nextHeadPosition();

    // Kolizja ze sciana.
    if (!board_.inside(nextHead))
    {
        state_ = State::GameOver;
        updateTexts();
        return;
    }

    const bool grow = nextHead == food_.position();
    snake_.move(grow);

    // Kolizja z wlasnym cialem.
    if (snake_.selfCollision())
    {
        state_ = State::GameOver;
        updateTexts();
        return;
    }

    if (grow)
    {
        ++score_;
        spawnFood();
    }

    if (state_ == State::Running)
    {
        updateTexts();
    }
}

void Game::render()
{
    window_.clear(sf::Color(18, 18, 18));

    if (state_ != State::EnterName)
    {
        // Rysujemy plansze tylko po wpisaniu nicku.
        for (const auto& segment : snake_.body())
        {
            snakeShape_.setPosition(
                {static_cast<float>(segment.x * config_.tileSize),
                 static_cast<float>(segment.y * config_.tileSize)});
            window_.draw(snakeShape_);
        }

        foodShape_.setPosition(
            {static_cast<float>(food_.position().x * config_.tileSize),
             static_cast<float>(food_.position().y * config_.tileSize)});
        window_.draw(foodShape_);

        window_.draw(scoreText_);
    }

    if (state_ == State::Paused)
    {
        window_.draw(pauseText_);
    }
    else if (state_ == State::EnterName)
    {
        window_.draw(promptText_);
        window_.draw(nameText_);
        window_.draw(instructionText_);
    }
    else if (state_ == State::GameOver)
    {
        window_.draw(gameOverText_);
        window_.draw(instructionText_);
        window_.draw(scoreboardText_);
    }

    window_.display();
}

void Game::reset()
{
    const int startX = std::clamp(board_.width() / 2, initialLength - 1, board_.width() - 1);
    snake_.reset({startX, board_.height() / 2}, initialLength, Direction::Right);
    pendingDirection_ = Direction::Right;
    score_ = 0;
    accumulator_ = 0.F;
    state_ = State::Running;
    highscoreRecorded_ = false;
    spawnFood();
    updateTexts();
}

void Game::spawnFood()
{
    food_.respawn(board_, snake_, random_);
}

void Game::loadHighscores()
{
    std::filesystem::create_directories(dataDir_);
    const auto path = dataDir_ / highscoreFile;

    highscores_.clear();
    if (!std::filesystem::exists(path))
    {
        return;
    }

    std::ifstream input(path);
    if (!input)
    {
        throw std::runtime_error("Failed to open highscore file: " + path.string());
    }

    std::string name;
    int score = 0;

    while (input >> name >> score)
    {
        for (char& ch : name)
        {
            if (ch >= 'a' && ch <= 'z')
            {
                ch = static_cast<char>(ch - ('a' - 'A'));
            }
        }
        highscores_.push_back({name, score});
    }

    normalizeHighscores(3);
}

void Game::saveHighscores()
{
    const auto path = dataDir_ / highscoreFile;
    std::ofstream output(path, std::ios::trunc);

    if (!output)
    {
        throw std::runtime_error("Failed to write highscore file: " + path.string());
    }

    for (const auto& entry : highscores_)
    {
        output << entry.name << " " << entry.score << "\n";
    }
}

bool Game::isOpposite(Direction next) const
{
    const Direction current = snake_.direction();

    return (current == Direction::Up && next == Direction::Down) ||
           (current == Direction::Down && next == Direction::Up) ||
           (current == Direction::Left && next == Direction::Right) ||
           (current == Direction::Right && next == Direction::Left);
}

void Game::updateTexts()
{
    const sf::Vector2f viewSize = window_.getView().getSize();
    const sf::Vector2f center{viewSize.x / 2.F, viewSize.y / 2.F};

    if (state_ == State::EnterName)
    {
        // Ekran wpisywania nicku
        promptText_.setString("ENTER YOUR NAME:");
        instructionText_.setString("PRESS ENTER TO START");
        const std::string shownName = nameInput_.empty() ? "_" : nameInput_;
        nameText_.setString(shownName);

        centerText(promptText_, {center.x, viewSize.y * 0.3F});
        centerText(nameText_, {center.x, viewSize.y * 0.45F});
        centerText(instructionText_, {center.x, viewSize.y * 0.6F});
    }
    else if (state_ == State::Paused)
    {
        pauseText_.setString("PAUSED");
        centerText(pauseText_, center);
    }
    else if (state_ == State::GameOver)
    {
        if (!highscoreRecorded_)
        {
            updateHighscores();
            highscoreRecorded_ = true;
        }

        // Ekran konca gry.
        gameOverText_.setString("GAME OVER");
        instructionText_.setString("Press R to restart or ESC to quit");

        std::ostringstream boardStream;
        boardStream << "SCOREBOARD:\n";
        if (highscores_.empty())
        {
            boardStream << "NONE";
        }
        else
        {
            const std::size_t count = std::min<std::size_t>(3, highscores_.size());
            for (std::size_t i = 0; i < count; ++i)
            {
                boardStream << highscores_[i].name << " " << highscores_[i].score;
                if (i + 1 < count)
                {
                    boardStream << "\n";
                }
            }
        }
        scoreboardText_.setString(boardStream.str());

        centerText(gameOverText_, {center.x, viewSize.y * 0.25F});
        centerText(instructionText_, {center.x, viewSize.y * 0.38F});
        centerText(scoreboardText_, {center.x, viewSize.y * 0.62F});
    }

    const int bestScore = highscores_.empty() ? 0 : highscores_.front().score;
    std::ostringstream scoreStream;
    scoreStream << "Score: " << score_ << "  Best: " << bestScore;
    scoreText_.setString(scoreStream.str());
}

void Game::updateNameInput(char32_t unicode)
{
    // Filtrujemy znaki tylko do A-Z, 0-9 i podkreslenia.
    if (unicode < 32 || unicode > 126)
    {
        return;
    }

    if (unicode >= U'a' && unicode <= U'z')
    {
        unicode = unicode - (U'a' - U'A');
    }

    if ((unicode >= U'A' && unicode <= U'Z') ||
        (unicode >= U'0' && unicode <= U'9') ||
        unicode == U'_')
    {
        if (nameInput_.size() < maxNameLength)
        {
            nameInput_.push_back(static_cast<char>(unicode));
        }
    }
}

void Game::updateHighscores()
{
    // Aktualizujemy wynik tylko dla biezacego gracza.
    if (playerName_.empty())
    {
        playerName_ = "PLAYER";
    }

    auto existing = std::find_if(highscores_.begin(), highscores_.end(),
                                 [&](const HighscoreEntry& entry)
                                 { return entry.name == playerName_; });

    if (existing == highscores_.end())
    {
        highscores_.push_back({playerName_, score_});
    }
    else if (score_ > existing->score)
    {
        existing->score = score_;
    }

    normalizeHighscores(3);
    saveHighscores();
}

void Game::normalizeHighscores(std::size_t limit)
{
    // Laczne wyniki dla tego samego nicku.
    std::vector<HighscoreEntry> merged;

    for (const auto& entry : highscores_)
    {
        auto existing = std::find_if(merged.begin(), merged.end(),
                                     [&](const HighscoreEntry& other)
                                     { return other.name == entry.name; });
        if (existing == merged.end())
        {
            merged.push_back(entry);
        }
        else if (entry.score > existing->score)
        {
            existing->score = entry.score;
        }
    }

    highscores_ = std::move(merged);
    std::sort(highscores_.begin(), highscores_.end(),
              [](const HighscoreEntry& lhs, const HighscoreEntry& rhs)
              { return lhs.score > rhs.score; });

    if (highscores_.size() > limit)
    {
        highscores_.resize(limit);
    }
}
