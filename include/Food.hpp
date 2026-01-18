#pragma once

#include "Board.hpp"
#include "Random.hpp"
#include "Snake.hpp"

// Przechowuje pozycje jedzenia i odswieza ja po zjedzeniu
class Food
{
public:
    const GridPos& position() const;
    // Losuje nowe polozenie na wolnym polu
    void respawn(const Board& board, const Snake& snake, Random& random);

private:
    GridPos position_{};
};
