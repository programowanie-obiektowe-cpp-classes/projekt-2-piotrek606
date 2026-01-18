#include "Snake.hpp"

#include <algorithm>
#include <iterator>
#include <ranges>

namespace
{
// Zamiana kierunku na przesuniecie na siatce
GridPos directionOffset(Direction direction)
{
    switch (direction)
    {
    case Direction::Up:
        return {0, -1};
    case Direction::Down:
        return {0, 1};
    case Direction::Left:
        return {-1, 0};
    case Direction::Right:
    default:
        return {1, 0};
    }
}
} // namespace

Snake::Snake(const GridPos& start, int initialLength, Direction direction)
{
    reset(start, initialLength, direction);
}

void Snake::reset(const GridPos& start, int initialLength, Direction direction)
{
    body_.clear();
    direction_ = direction;

    // Ustawiamy ogon za glowa na osi X.
    for (int i = 0; i < initialLength; ++i)
    {
        body_.push_back({start.x - i, start.y});
    }
}

const std::deque<GridPos>& Snake::body() const
{
    return body_;
}

GridPos Snake::head() const
{
    return body_.front();
}

Direction Snake::direction() const
{
    return direction_;
}

void Snake::setDirection(Direction direction)
{
    direction_ = direction;
}

GridPos Snake::nextHeadPosition() const
{
    return head() + directionOffset(direction_);
}

void Snake::move(bool grow)
{
    body_.push_front(nextHeadPosition());

    if (!grow)
    {
        body_.pop_back();
    }
}

bool Snake::occupies(const GridPos& pos) const
{
    return std::ranges::find(body_, pos) != body_.end();
}

bool Snake::selfCollision() const
{
    if (body_.size() < 2)
    {
        return false;
    }

    // Glowa nie moze wchodzic w reszte ciala
    const auto& headPos = body_.front();
    return std::ranges::any_of(std::next(body_.begin()), body_.end(),
                               [&](const GridPos& segment)
                               { return segment == headPos; });
}
