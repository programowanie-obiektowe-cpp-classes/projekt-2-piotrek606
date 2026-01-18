#include "Board.hpp"

Board::Board(int width, int height)
    : width_(width), height_(height)
{
}

int Board::width() const
{
    return width_;
}

int Board::height() const
{
    return height_;
}

bool Board::inside(const GridPos& pos) const
{
    // Sprawdza czy pozycja jest wewnatrz planszy
    return pos.x >= 0 && pos.x < width_ && pos.y >= 0 && pos.y < height_;
}
