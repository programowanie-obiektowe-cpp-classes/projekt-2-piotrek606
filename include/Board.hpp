#pragma once

#include "Types.hpp"

// Parametry planszy i prosta walidacja polozenia.
class Board
{
public:
    Board(int width, int height);

    int width() const;
    int height() const;
    // Sprawdza czy pozycja miesci sie w planszy.
    bool inside(const GridPos& pos) const;

private:
    int width_{};
    int height_{};
};
