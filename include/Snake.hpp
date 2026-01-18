#pragma once

#include "Types.hpp"

#include <deque>

// Kierunek ruchu weza.
enum class Direction
{
    Up,
    Down,
    Left,
    Right
};

// Logika weza niezalezna od grafiki.
class Snake
{
public:
    Snake(const GridPos& start, int initialLength, Direction direction);

    void reset(const GridPos& start, int initialLength, Direction direction);

    const std::deque<GridPos>& body() const;
    GridPos head() const;

    Direction direction() const;
    void setDirection(Direction direction);

    GridPos nextHeadPosition() const;
    // Przesuwa weza, opcjonalnie wydluzajac cialo.
    void move(bool grow);

    // Sprawdza czy waz zajmuje dane pole.
    bool occupies(const GridPos& pos) const;
    // Sprawdza zderzenie glowy z cialem.
    bool selfCollision() const;

private:
    std::deque<GridPos> body_;
    Direction direction_{Direction::Right};
};
