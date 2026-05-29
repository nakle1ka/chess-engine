#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "./run_tests.hpp"
#include "../internal/attacks/attacks.hpp"
#include "../internal/shared/utils.hpp"
#include "../internal/shared/types.hpp"
#include "../internal/shared/constants.hpp"

using namespace std;

class RaysMock : public Rays {
public:
    bitboard get_ray(DIRECTION dir, int sq) {
        if (sq == 36 && dir == DIRECTION::NORTH) {
            return 0x1010100000000000ULL; 
        }
        return 0ULL;
    }
};

int main() {
    RaysMock mock_rays;
    Attacks attacks(&mock_rays);

    vector<TestCase<bitboard>> pawn_advances_tests = {
        { attacks.get_pawn_advances(COLORS::WHITE, 0x1000ULL, 0ULL), 0x10100000ULL },
        { attacks.get_pawn_advances(COLORS::WHITE, 0x1000ULL, 0x100000ULL), 0ULL },
        { attacks.get_pawn_advances(COLORS::WHITE, 0x1000ULL, 0x10000000ULL), 0x100000ULL },
        { attacks.get_pawn_advances(COLORS::BLACK, 0x0008000000000000ULL, 0ULL), 0x0000080800000000ULL },
        { attacks.get_pawn_advances(COLORS::WHITE, 0x0100000000000000ULL, 0ULL), 0ULL }
    };
    RunTests("PAWN ADVANCES", pawn_advances_tests);

    vector<TestCase<bitboard>> pawn_attacks_tests = {
        { attacks.get_pawn_attacks(COLORS::WHITE, 0x1000ULL), 0x280000ULL },
        { attacks.get_pawn_attacks(COLORS::WHITE, 0x100ULL), 0x20000ULL },
        { attacks.get_pawn_attacks(COLORS::BLACK, 0x80000000000000ULL), 0x400000000000ULL }
    };
    RunTests("PAWN ATTACKS", pawn_attacks_tests);

    vector<TestCase<bitboard>> knight_attacks_tests = {
        { attacks.get_knights_attacks(0x800000000ULL), 0x14220022140000ULL },
        { attacks.get_knights_attacks(1ULL), 0x20400ULL },
        { attacks.get_knights_attacks(0x8000000000000000ULL), 9077567998918656ULL }
    };
    RunTests("KNIGHT ATTACKS", knight_attacks_tests);

    vector<TestCase<bitboard>> king_attacks_tests = {
        { attacks.get_king_attacks(0x1000000000ULL), 0x382838000000ULL },
        { attacks.get_king_attacks(0x80ULL), 0xC040ULL },
        { attacks.get_king_attacks(0x1000000ULL), 0x0302030000ULL }
    };
    RunTests("KING ATTACKS", king_attacks_tests);

    return 0;
}
