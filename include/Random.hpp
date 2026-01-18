#pragma once

#include <random>

// Prosty generator liczb losowych dla gry.
class Random
{
public:
    Random();

    // Losuje liczbe calkowita z zakresu [min, max].
    int uniformInt(int min, int max);

private:
    std::mt19937 engine_;
};
