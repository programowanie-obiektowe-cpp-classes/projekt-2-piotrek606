#pragma once

#include <compare>

// Pozycja na siatce planszy.
struct GridPos
{
    int x{};
    int y{};

    auto operator<=>(const GridPos&) const = default;
};

inline GridPos operator+(const GridPos& lhs, const GridPos& rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}
