#include "./chess_visual.hpp"
#include "../shared/move.hpp"

ChessVisual::ChessVisual(Board *_board, AI *_ai) : board(_board), ai(_ai) {};

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
            auto [piece, color] = board->piece_at(square);

            if (piece == PIECE_TYPE::NONE)
            {
                if ((rank + file) % 2 == 0)
                    std::cout << " # ";
                else
                    std::cout << "   ";
            }
            else
            {
                bool is_white = (color == COLORS::WHITE);
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
    move.set_from(from);
    move.set_to(to);
    move.set_color(board->get_turn_color());
    move.set_piece(board->piece_at(from).first);
    move.set_captured(board->piece_at(to).first);
    move.set_promotion(PIECE_TYPE::NONE);
    move.set_type(determine_move_type(move, input));

    return move;
}

MOVE_TYPE ChessVisual::determine_move_type(Move &move, const std::string &input)
{
    if (move.get_piece() == PIECE_TYPE::KING && abs(move.get_to() - move.get_from()) == 2)
        return MOVE_TYPE::CASTLE;

    if (move.get_piece() == PIECE_TYPE::PAWN)
    {
        int target_rank = (move.get_color() == COLORS::WHITE) ? 7 : 0;
        if (move.get_to() / 8 == target_rank)
        {
            PIECE_TYPE promotion = PIECE_TYPE::QUEEN;
            if (input.length() > 4)
            {
                char prom_char = input[4];
                switch (tolower(prom_char))
                {
                case 'q':
                    promotion = PIECE_TYPE::QUEEN;
                    break;
                case 'r':
                    promotion = PIECE_TYPE::ROOK;
                    break;
                case 'b':
                    promotion = PIECE_TYPE::BISHOP;
                    break;
                case 'n':
                    promotion = PIECE_TYPE::KNIGHT;
                    break;
                default:
                    promotion = PIECE_TYPE::QUEEN;
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
                    promotion = PIECE_TYPE::QUEEN;
                    break;
                case 'r':
                    promotion = PIECE_TYPE::ROOK;
                    break;
                case 'b':
                    promotion = PIECE_TYPE::BISHOP;
                    break;
                case 'n':
                    promotion = PIECE_TYPE::KNIGHT;
                    break;
                default:
                    promotion = PIECE_TYPE::QUEEN;
                }
            }
            move.set_promotion(promotion);
            return MOVE_TYPE::PROMOTION;
        }
    }

    if (move.get_piece() == PIECE_TYPE::PAWN && move.get_captured() == PIECE_TYPE::NONE)
    {
        int file_diff = abs((move.get_from() % 8) - (move.get_to() % 8));
        if (file_diff == 1)
        {
            return MOVE_TYPE::EN_PASSANT;
        }
    }

    if (move.get_captured() != PIECE_TYPE::NONE)
    {
        return MOVE_TYPE::CAPTURE;
    }

    return MOVE_TYPE::NORMAL;
}

void ChessVisual::play()
{
    std::string input;
    display();

    while (true)
    {
        COLORS turn_color = board->get_turn_color();
        COLORS opponent_color = turn_color == COLORS::WHITE ? COLORS::BLACK : COLORS::WHITE;

        std::cout << (turn_color == COLORS::WHITE ? "White" : "Black") << "'s turn: ";
        std::getline(std::cin, input);

        if (input == "quit" || input == "exit")
        {
            break;
        }

        if (input == "undo")
        {

            board->undo_move();
            continue;
        }

        Move move = parse_move(input);
        bool moved = board->make_move(move);

        if (!moved)
        {
            std::cout << "Invalid move\n";
            continue;
        }

        display();

        if (board->is_game_end())
        {
            if (board->is_king_checked(opponent_color))
                std::cout << (turn_color == COLORS::WHITE ? "White's " : "Black's ") << "win!\n";
            else
                std::cout << "Stalemate!\n";
            break;
        }

        Move alg_move = ai->get_best_move(5);
        board->ugly_move(alg_move);

        display();

        if (board->is_game_end())
        {
            if (board->is_king_checked(turn_color))
                std::cout << (opponent_color == COLORS::WHITE ? "White`s " : "Black`s ") << "win!\n";
            else
                std::cout << "Stalemate!\n";
            break;
        }
    }
}