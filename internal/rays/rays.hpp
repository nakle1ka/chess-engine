#pragma once

#include "../shared/types.hpp"

class Rays
{
private:
    bitboard rays[8][64];

public:
    Rays();
    bitboard get_ray(DIRECTION dir, int sq) const;
};
