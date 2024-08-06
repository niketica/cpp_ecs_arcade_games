#pragma once

#include <algorithm>
#include <random>

class RandomNumberGenerator
{
public:
    RandomNumberGenerator() = default;

    int getRandomNumber(const int highest) const;

    int getRandomNumber(const int lowest, const int highest) const;
};