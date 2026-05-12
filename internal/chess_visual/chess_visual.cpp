#include "./chess_visual.hpp"

ChessVisual::ChessVisual(Board *_board)
{
    board = _board;
}

int ChessVisual::piece_to_index(PIECE_TYPE piece)
{
    switch (piece)
    {
    case PIECE_TYPE::KING:
        return 0;
    case PIECE_TYPE::QUEEN:
        return 1;
    case PIECE_TYPE::ROOK:
        return 2;
    case PIECE_TYPE::BISHOP:
        return 3;
    case PIECE_TYPE::KNIGHT:
        return 4;
    case PIECE_TYPE::PAWN:
        return 5;
    default:
        return -1;
    }
}

PIECE_TYPE ChessVisual::get_piece_at(int square)
{
    bitboard sq_bit = 1ULL << square;

    if (board->getWhiteKing() & sq_bit)
        return PIECE_TYPE::KING;
    if (board->getWhiteQueen() & sq_bit)
        return PIECE_TYPE::QUEEN;
    if (board->getWhiteRooks() & sq_bit)
        return PIECE_TYPE::ROOK;
    if (board->getWhiteBishops() & sq_bit)
        return PIECE_TYPE::BISHOP;
    if (board->getWhiteKnights() & sq_bit)
        return PIECE_TYPE::KNIGHT;
    if (board->getWhitePawns() & sq_bit)
        return PIECE_TYPE::PAWN;

    if (board->getBlackKing() & sq_bit)
        return PIECE_TYPE::KING;
    if (board->getBlackQueen() & sq_bit)
        return PIECE_TYPE::QUEEN;
    if (board->getBlackRooks() & sq_bit)
        return PIECE_TYPE::ROOK;
    if (board->getBlackBishops() & sq_bit)
        return PIECE_TYPE::BISHOP;
    if (board->getBlackKnights() & sq_bit)
        return PIECE_TYPE::KNIGHT;
    if (board->getBlackPawns() & sq_bit)
        return PIECE_TYPE::PAWN;

    return PIECE_TYPE::NONE;
}

COLORS ChessVisual::get_color_at(int square)
{
    bitboard sq_bit = 1ULL << square;
    bitboard white_pieces = board->getWhiteKing() | board->getWhiteQueen() |
                            board->getWhiteRooks() | board->getWhiteBishops() |
                            board->getWhiteKnights() | board->getWhitePawns();

    return (white_pieces & sq_bit) ? COLORS::WHITE : COLORS::BLACK;
}

void ChessVisual::display()
{

#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    std::cout << "\n  +------------------------+\n";

    for (int rank = 7; rank >= 0; rank--)
    {
        std::cout << (rank + 1) << " |";

        for (int file = 0; file < 8; file++)
        {
            int square = rank * 8 + file;
            PIECE_TYPE piece = get_piece_at(square);

            if (piece == PIECE_TYPE::NONE)
            {
                if ((rank + file) % 2 == 0)
                    std::cout << " # ";
                else
                    std::cout << "   ";
            }
            else
            {
                bool is_white = (get_color_at(square) == COLORS::WHITE);
                int index = piece_to_index(piece);
                std::cout << " " << PIECE_SYMBOLS[is_white ? 0 : 1][index] << " ";
            }
        }

        std::cout << "|\n";
    }

    std::cout << "  +------------------------+\n";
    std::cout << "    a  b  c  d  e  f  g  h\n\n";
}

Move ChessVisual::parse_move(const std::string &input)
{
    if (input.length() < 4)
    {
        throw std::invalid_argument("Invalid input format");
    }

    int from_file = input[0] - 'a';
    int from_rank = input[1] - '1';
    int to_file = input[2] - 'a';
    int to_rank = input[3] - '1';

    if (from_file < 0 || from_file > 7 || from_rank < 0 || from_rank > 7 ||
        to_file < 0 || to_file > 7 || to_rank < 0 || to_rank > 7)
    {
        throw std::invalid_argument("Coordinates out of range");
    }

    int from = from_rank * 8 + from_file;
    int to = to_rank * 8 + to_file;

    Move move;
    move.from = from;
    move.to = to;
    move.color = board->get_turn_color();
    move.piece = get_piece_at(from);
    move.captured = get_piece_at(to);
    move.promoutioned = PIECE_TYPE::NONE;
    move.type = determine_move_type(move, input);

    return move;
}

MOVE_TYPE ChessVisual::determine_move_type(Move &move, const std::string &input)
{
    if (move.piece == PIECE_TYPE::KING && abs(move.to - move.from) == 2)
        return MOVE_TYPE::CASTLE;

    if (move.piece == PIECE_TYPE::PAWN)
    {
        int target_rank = (move.color == COLORS::WHITE) ? 7 : 0;
        if (move.to / 8 == target_rank)
        {
            if (input.length() > 4)
            {
                char prom_char = input[4];
                switch (tolower(prom_char))
                {
                case 'q':
                    move.promoutioned = PIECE_TYPE::QUEEN;
                    break;
                case 'r':
                    move.promoutioned = PIECE_TYPE::ROOK;
                    break;
                case 'b':
                    move.promoutioned = PIECE_TYPE::BISHOP;
                    break;
                case 'n':
                    move.promoutioned = PIECE_TYPE::KNIGHT;
                    break;
                default:
                    move.promoutioned = PIECE_TYPE::QUEEN;
                }
            }
            else
            {
                std::cout << "Choose promotion (q/r/b/n): ";
                char prom_char;
                std::cin >> prom_char;
                switch (prom_char)
                {
                case 'q':
                    move.promoutioned = PIECE_TYPE::QUEEN;
                    break;
                case 'r':
                    move.promoutioned = PIECE_TYPE::ROOK;
                    break;
                case 'b':
                    move.promoutioned = PIECE_TYPE::BISHOP;
                    break;
                case 'n':
                    move.promoutioned = PIECE_TYPE::KNIGHT;
                    break;
                default:
                    move.promoutioned = PIECE_TYPE::QUEEN;
                }
            }
            return MOVE_TYPE::PROMOTION;
        }
    }

    if (move.piece == PIECE_TYPE::PAWN && move.captured == PIECE_TYPE::NONE)
    {
        int file_diff = abs((move.from % 8) - (move.to % 8));
        if (file_diff == 1)
        {
            return MOVE_TYPE::EN_PASSANT;
        }
    }

    if (move.captured != PIECE_TYPE::NONE)
    {
        return MOVE_TYPE::CAPTURE;
    }

    return MOVE_TYPE::NORMAL;
}

void ChessVisual::play()
{
    std::string input;

    while (true)
    {
        display();

        std::cout << (board->get_turn_color() == COLORS::WHITE ? "White" : "Black") << "'s turn: ";
        std::getline(std::cin, input);

        if (input == "quit" || input == "exit")
        {
            break;
        }

        if (input == "undo")
        {
            try
            {
                board->undo_move();
            }
            catch (...)
            {
                std::cout << "Cannot undo move!\n";
            }
            continue;
        }

        Move move = parse_move(input);
        bool moved = board->make_move(move);

        if (!moved)
        {
            std::cout << "Invalid move\n";
        }
    }
}