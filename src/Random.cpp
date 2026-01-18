#include "Random.hpp"

#include <chrono>
#include <random>

Random::Random()
    : engine_(static_cast<std::mt19937::result_type>(
          std::chrono::steady_clock::now().time_since_epoch().count()))
{
    // Seed z zegara wystarczy do gry.
}

int Random::uniformInt(int min, int max)
{
    std::uniform_int_distribution<int> dist(min, max);
    return dist(engine_);
}
