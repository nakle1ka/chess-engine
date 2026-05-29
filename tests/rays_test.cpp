#include <iostream>
#include <vector>
#include <string>

#include "./run_tests.hpp"
#include "../internal/rays/rays.hpp"
#include "../internal/shared/utils.hpp"
#include "../internal/shared/types.hpp"
#include "../internal/shared/constants.hpp"

int main() {
    Rays rays_obj;

    vector<TestCase<bitboard>> north_tests = {
        { rays_obj.get_ray(DIRECTION::NORTH, 0), 0x0101010101010100ULL },
        { rays_obj.get_ray(DIRECTION::NORTH, 36), 0x1010100000000000ULL },
        { rays_obj.get_ray(DIRECTION::NORTH, 56), 0ULL }
    };
    RunTests("NORTH RAYS", north_tests);

    vector<TestCase<bitboard>> south_tests = {
        { rays_obj.get_ray(DIRECTION::SOUTH, 63), 0x80808080808080ULL },
        { rays_obj.get_ray(DIRECTION::SOUTH, 36), 0x10101010ULL },
        { rays_obj.get_ray(DIRECTION::SOUTH, 4), 0ULL }
    };
    RunTests("SOUTH RAYS", south_tests);

    vector<TestCase<bitboard>> west_tests = {
        { rays_obj.get_ray(DIRECTION::WEST, 0), 0ULL },
        { rays_obj.get_ray(DIRECTION::WEST, 36), 0xF00000000ULL },
        { rays_obj.get_ray(DIRECTION::WEST, 63), 0x7F00000000000000ULL }
    };
    RunTests("WEST RAYS", west_tests);

    vector<TestCase<bitboard>> east_tests = {
        { rays_obj.get_ray(DIRECTION::EAST, 0), 0xFEULL },
        { rays_obj.get_ray(DIRECTION::EAST, 36), 0xE000000000ULL },
        { rays_obj.get_ray(DIRECTION::EAST, 63), 0ULL }
    };
    RunTests("EAST RAYS", east_tests);

    vector<TestCase<bitboard>> north_east_tests = {
        { rays_obj.get_ray(DIRECTION::NORTH_EAST, 0), 0x8040201008040200ULL },
        { rays_obj.get_ray(DIRECTION::NORTH_EAST, 36), 0x8040200000000000ULL },
        { rays_obj.get_ray(DIRECTION::NORTH_EAST, 7), 0ULL }
    };
    RunTests("NORTH EAST RAYS", north_east_tests);

    vector<TestCase<bitboard>> north_west_tests = {
        { rays_obj.get_ray(DIRECTION::NORTH_WEST, 7), 0x0102040810204000ULL },
        { rays_obj.get_ray(DIRECTION::NORTH_WEST, 36), 0x0204080000000000ULL },
        { rays_obj.get_ray(DIRECTION::NORTH_WEST, 0), 0ULL }
    };
    RunTests("NORTH WEST RAYS", north_west_tests);

    vector<TestCase<bitboard>> south_east_tests = {
        { rays_obj.get_ray(DIRECTION::SOUTH_EAST, 56), 0x02040810204080ULL },
        { rays_obj.get_ray(DIRECTION::SOUTH_EAST, 36), 0x20408000ULL },
        { rays_obj.get_ray(DIRECTION::SOUTH_EAST, 7), 0ULL }
    };
    RunTests("SOUTH EAST RAYS", south_east_tests);

    vector<TestCase<bitboard>> south_west_tests = {
        { rays_obj.get_ray(DIRECTION::SOUTH_WEST, 63), 0x40201008040201ULL },
        { rays_obj.get_ray(DIRECTION::SOUTH_WEST, 36), 0x08040201ULL },
        { rays_obj.get_ray(DIRECTION::SOUTH_WEST, 56), 0ULL }
    };
    RunTests("SOUTH WEST RAYS", south_west_tests);

    return 0;
}
