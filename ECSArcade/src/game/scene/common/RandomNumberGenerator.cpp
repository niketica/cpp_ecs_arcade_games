#include "RandomNumberGenerator.h"

int RandomNumberGenerator::getRandomNumber(const int highest) const
{
    return getRandomNumber(0, highest);
}

int RandomNumberGenerator::getRandomNumber(const int lowest, const int highest) const
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(lowest, highest);
    return dis(gen);
}