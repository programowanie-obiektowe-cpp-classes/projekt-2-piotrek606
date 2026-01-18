#include "Food.hpp"

#include <vector>

const GridPos& Food::position() const
{
    return position_;
}

void Food::respawn(const Board& board, const Snake& snake, Random& random)
{
    // Zbieramy wszystkie wolne pola planszy
    std::vector<GridPos> freeFields;
    freeFields.reserve(static_cast<std::size_t>(board.width() * board.height()));

    for (int y = 0; y < board.height(); ++y)
    {
        for (int x = 0; x < board.width(); ++x)
        {
            GridPos pos{x, y};
            if (!snake.occupies(pos))
            {
                freeFields.push_back(pos);
            }
        }
    }

    if (freeFields.empty())
    {
        return;
    }

    const int index = random.uniformInt(0, static_cast<int>(freeFields.size()) - 1);
    position_ = freeFields[static_cast<std::size_t>(index)];
}
