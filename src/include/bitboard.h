#include<cstdint>
#include<cstddef>
#include<vector>

const size_t BOARDSIZE = 8;
const uint64_t MASK_OFF_LEFT = 0x7F7F7F7F7F7F7F7F;
const uint64_t MASK_OFF_RIGHT = 0xFEFEFEFEFEFEFEFE;
const uint64_t MASK_OFF_LEFT_DOUBLE = 0x3F3F3F3F3F3F3F3F;
const uint64_t MASK_OFF_RIGHT_DOUBLE = 0xFCFCFCFCFCFCFCFC;
const uint64_t MASK_OFF_BOTTOM = 0x00FFFFFFFFFFFFFF;
const uint64_t MASK_OFF_TOP = 0xFFFFFFFFFFFFFF00;
const uint64_t MASK_OFF_BOTTOM_DOUBLE = 0x0000FFFFFFFFFFFF;
const uint64_t MASK_OFF_TOP_DOUBLE = 0xFFFFFFFFFFFF0000;

constexpr size_t compute_distance(size_t row, size_t col){
    return row * BOARDSIZE + col;
}

constexpr uint64_t start_pawns(size_t row){
    uint64_t val{0};
    for(int j=0;j<BOARDSIZE;++j){
        uint64_t mask{1};
        mask <<= compute_distance(row,j);
        val |= mask;
    }
    return val;
}

constexpr uint64_t start_two(size_t row, size_t edge_offset){
    uint64_t val{0};
    uint64_t mask{1};
    mask <<= compute_distance(row, edge_offset);
    val |= mask;

    mask = 1;
    mask <<= compute_distance(row,BOARDSIZE-1-edge_offset);
    val |= mask;
    return val;
}

constexpr uint64_t start_king(size_t row){
    uint64_t val{0};
    uint64_t mask{1};
    mask <<= compute_distance(row, 4);
    val |= mask;
    return val;
}

constexpr uint64_t start_queen(size_t row){
    uint64_t val{0};
    uint64_t mask{1};
    mask <<= compute_distance(row, 3);
    val |= mask;
    return val;
}

constexpr uint64_t start_rook(size_t row){
    return start_two(row,0);
}

constexpr uint64_t start_knight(size_t row){
    return start_two(row,1);
}

constexpr uint64_t start_bishop(size_t row){
    return start_two(row,2);
}

const size_t PAWN_OFFSET = 0;
const size_t ROOK_OFFSET = 1;
const size_t BISHOP_OFFSET = 2;
const size_t KNIGHT_OFFSET = 3;
const size_t KING_OFFSET = 4;
const size_t QUEEN_OFFSET = 5;

struct bitboard_move{
    int distance;
    uint64_t board;
};

struct bitboard_player_set{
    uint64_t pawns;
    uint64_t rooks;
    uint64_t bishops;
    uint64_t knights;
    uint64_t king;
    uint64_t queen;
#ifdef UNITTEST
    uint64_t barrier;
#endif

    bitboard_player_set(bool opponent=false);
    uint64_t full_player_board();

    void remove_piece(size_t struct_offset, size_t absolute_position);
    void remove_pieces(size_t absolute_position);
    void move_piece(size_t struct_offset, size_t from_position, size_t to_position);
};

struct ascii_array{
    char data[64];
};

const size_t PLAYER_OFFSET = 0;
const size_t OPPONENT_OFFSET = 1;

struct bitboard_frame{
    bitboard_player_set player;
    bitboard_player_set opponent;
    bitboard_frame(bitboard_player_set player, bitboard_player_set opponent);
    ascii_array to_ascii_array();
    std::vector<bitboard_frame> get_next_boards();
    std::vector<bitboard_frame> get_opponent_next_boards();
    bitboard_frame clone_from_player_move(size_t struct_offset, size_t from_position, size_t to_position);
    bitboard_frame clone_from_opponent_move(size_t struct_offset, size_t from_position, size_t to_position);
    bitboard_frame remove_pieces(size_t struct_offset, size_t position);
};