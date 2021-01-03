#include <gtest/gtest.h>
#include <array>
#include <set>
#include <vector>
#include "bitboard.h"

size_t count_bits(uint64_t board){
    size_t count{0};
    while(board){
        if(board&1)
            ++count;
        board>>=1;
    }
    return count;
}


std::set<uint64_t> get_player_board_set(const std::vector<bitboard_frame>& moves, size_t struct_offset){
    std::set<uint64_t> board_set;
    for(auto b: moves){
        board_set.emplace(reinterpret_cast<uint64_t*>(&b)[struct_offset]);
    }
    return board_set;
}

std::set<uint64_t> get_opp_board_set(const std::vector<bitboard_frame>& moves, size_t struct_offset){
    std::set<uint64_t> board_set;
    for(auto b: moves){
        board_set.emplace(reinterpret_cast<uint64_t*>(&b.opponent)[struct_offset]);
    }
    return board_set;
}

TEST(bitboard, compute_distance)
{
    GTEST_ASSERT_EQ(compute_distance(1, 1), 9);
    GTEST_ASSERT_EQ(compute_distance(0, 0), 0);
    GTEST_ASSERT_EQ(compute_distance(1, 0), 8);
    GTEST_ASSERT_EQ(compute_distance(0, 1), 1);
    GTEST_ASSERT_EQ(compute_distance(2, 2), 18);
    GTEST_ASSERT_EQ(compute_distance(2, 1), 17);
    GTEST_ASSERT_EQ(compute_distance(1, 2), 10);
    GTEST_ASSERT_EQ(compute_distance(0, 7), 7);
    GTEST_ASSERT_EQ(compute_distance(7, 0), 56);
    GTEST_ASSERT_EQ(compute_distance(7, 7), 63);
}

TEST(bitboard, move)
{
    bitboard_player_set bb;
    GTEST_ASSERT_GT(bb.pawns &(uint64_t)(1)<<9,0);
    GTEST_ASSERT_EQ(bb.pawns &(uint64_t)(1)<<1,0);
    GTEST_ASSERT_EQ(count_bits(bb.pawns),8);

    bb.move_piece((size_t)PAWN_OFFSET, (size_t)9, (size_t)1);
    GTEST_ASSERT_EQ(bb.pawns &(uint64_t)(1)<<9,0);
    GTEST_ASSERT_GT(bb.pawns &(uint64_t)(1)<<1,0);
    GTEST_ASSERT_EQ(count_bits(bb.pawns),8);
}

TEST(bitboard, remove)
{
    bitboard_player_set bb;
    GTEST_ASSERT_GT(bb.pawns &(uint64_t)(1)<<9,0);
    GTEST_ASSERT_EQ(bb.pawns &(uint64_t)(1)<<1,0);
    GTEST_ASSERT_EQ(count_bits(bb.pawns),8);

    bb.remove_piece((size_t)PAWN_OFFSET, (size_t)9);
    GTEST_ASSERT_EQ(bb.pawns &(uint64_t)(1)<<9,0);
    GTEST_ASSERT_EQ(count_bits(bb.pawns),7);

    bb.remove_piece((size_t)PAWN_OFFSET, (size_t)1);
    GTEST_ASSERT_EQ(bb.pawns &(uint64_t)(1)<<1,0);
    GTEST_ASSERT_EQ(count_bits(bb.pawns),7);
}

TEST(bitboard, start_vals)
{
    size_t row = 1;
    auto board = start_pawns(row);
    for(int i =0; i< 8; ++i){
        GTEST_ASSERT_EQ(board&1, 0);
        board>>=1;
    }
    for(int i =0; i< (8*row); ++i){
        GTEST_ASSERT_EQ(board&1, 1);
        board>>=1;
    }
    GTEST_ASSERT_EQ(board, 0);

    row = 7;
    board = start_pawns(row);
    for(int i =0; i< (8*row); ++i){
        GTEST_ASSERT_EQ(board&1, 0);
        board>>=1;
    }
    for(int i =0; i< 8; ++i){
        GTEST_ASSERT_EQ(board&1, 1);
        board>>=1;
    }

    std::vector test_bit_data {
        std::make_tuple(start_rook(0),std::set<int>{0,7}),
        std::make_tuple(start_knight(0),std::set<int>{1,6}),
        std::make_tuple(start_bishop(0),std::set<int>{2,5}),
        std::make_tuple(start_queen(0),std::set<int>{3}),
        std::make_tuple(start_king(0),std::set<int>{4})
    };

    for(auto[result,expected_spaces] : test_bit_data){
        for(int i=0; i<8;++i){
            if(expected_spaces.find(i) != expected_spaces.end()){
                if((result&1) != 1)
                    GTEST_ASSERT_EQ(result&1, 1);
                GTEST_ASSERT_EQ(result&1, 1);
            }
            else{
                if((result&1) == 1)
                    GTEST_ASSERT_EQ(result&1, 1);
                GTEST_ASSERT_EQ(result&1, 0);
            }
            result >>= 1;
        }
    }
}

TEST(bitboard, test_pawn_moves)
{
    bitboard_player_set bb;
    bb.rooks = 0;
    bb.knights = 0;
    bb.bishops = 0;
    bb.king = 0;
    bb.queen = 0;
    bitboard_player_set bbo(true);
    bitboard_frame frm(bb,bbo);

    auto moves = frm.get_next_boards();
    GTEST_ASSERT_EQ(moves.size(), 16);
    GTEST_ASSERT_EQ(count_bits(frm.player.pawns), 8);


    //test two space basic moves
    frm.player.pawns = ((uint64_t) 1)<<8;
    moves = frm.get_next_boards();
    GTEST_ASSERT_EQ(moves.size(), 2);
    GTEST_ASSERT_EQ(count_bits(frm.player.pawns), 1);
    std::set<uint64_t> board_lookup {get_player_board_set(moves, PAWN_OFFSET)};
    GTEST_ASSERT_NE(board_lookup.find(((uint64_t) 1)<<16), board_lookup.end());
    GTEST_ASSERT_NE(board_lookup.find(((uint64_t) 1)<<24), board_lookup.end());
    GTEST_ASSERT_EQ(board_lookup.find(((uint64_t) 1)<<15), board_lookup.end());
    GTEST_ASSERT_EQ(board_lookup.find(((uint64_t) 1)<<17), board_lookup.end());

    //test only one space available away from starting row
    frm.opponent.pawns = 0;
    frm.opponent.rooks = 0;
    frm.opponent.knights = 0;
    for(auto v : std::vector<size_t>{16,24,32,40,48}){
        frm.player.pawns = ((uint64_t) 1)<<v;
        moves = frm.get_next_boards();
        GTEST_ASSERT_EQ(moves.size(), 1);
        GTEST_ASSERT_EQ(count_bits(frm.player.pawns), 1);
        GTEST_ASSERT_EQ(count_bits(moves[0].player.pawns), 1);
        GTEST_ASSERT_EQ(moves[0].player.pawns, ((uint64_t) 1)<<(v+8));
    }
    //no moves at edge
    frm.player.pawns = ((uint64_t) 1)<<56;
    moves = frm.get_next_boards();
    GTEST_ASSERT_EQ(moves.size(), 0);

    //test no moves if blocked on front
    frm.player.pawns = ((uint64_t) 1)<<8;
    frm.opponent.pawns = ((uint64_t) 1)<<16;
    moves = frm.get_next_boards();
    GTEST_ASSERT_EQ(moves.size(), 0);
    GTEST_ASSERT_EQ(count_bits(frm.player.pawns), 1);
    GTEST_ASSERT_EQ(count_bits(frm.opponent.pawns), 1);

    //test move available if strikeable added
    frm.opponent.pawns |= ((uint64_t) 1)<<17;
    moves = frm.get_next_boards();
    GTEST_ASSERT_EQ(moves.size(), 1);
    GTEST_ASSERT_EQ(count_bits(frm.player.pawns), 1);
    GTEST_ASSERT_EQ(count_bits(frm.opponent.pawns), 2);
    GTEST_ASSERT_EQ(moves[0].player.pawns, ((uint64_t) 1)<<17);

    //test moves does not strike on same row
    frm.opponent.pawns = ((uint64_t) 1)<<15;
    moves = frm.get_next_boards();
    GTEST_ASSERT_EQ(moves.size(), 2);
    GTEST_ASSERT_EQ(count_bits(frm.player.pawns), 1);
    for(auto m: moves){
        GTEST_ASSERT_EQ(count_bits(m.player.pawns), 1);
        GTEST_ASSERT_EQ(count_bits(m.opponent.pawns), 1);
    }

    //test all three moves available when strike is present
    frm.opponent.pawns = ((uint64_t) 1)<<17;
    moves = frm.get_next_boards();
    GTEST_ASSERT_EQ(moves.size(), 3);
    GTEST_ASSERT_EQ(count_bits(frm.player.pawns), 1);
    size_t strike_moves = 0;
    for(auto m: moves){
        GTEST_ASSERT_EQ(count_bits(m.player.pawns), 1);
        if(m.opponent.pawns){
            GTEST_ASSERT_EQ(count_bits(m.opponent.pawns), 1);
        }
        else{
             GTEST_ASSERT_EQ(m.player.pawns, ((uint64_t) 1)<<17);
             ++strike_moves;
        }
    }
    GTEST_ASSERT_EQ(strike_moves, 1);

    //test strikeable 
    frm.player.pawns = ((uint64_t) 1)<<11;
    frm.opponent.pawns = ((uint64_t) 1)<<18;
    moves = frm.get_next_boards();
    board_lookup = get_player_board_set(moves, PAWN_OFFSET);
    GTEST_ASSERT_EQ(moves.size(), 3);
    GTEST_ASSERT_EQ(count_bits(frm.player.pawns), 1);
    strike_moves = 0;
    for(auto m: moves){
        GTEST_ASSERT_EQ(count_bits(m.player.pawns), 1);
        if(m.opponent.pawns){
            GTEST_ASSERT_EQ(count_bits(m.opponent.pawns), 1);
        }
        else{
             GTEST_ASSERT_GT(m.player.pawns & ((uint64_t) 1)<<18, 0);
             ++strike_moves;
        }
    }
    GTEST_ASSERT_EQ(strike_moves, 1);

    //test strikeable from  two pawns
    frm.player.pawns |= ((uint64_t) 1)<<9;
    moves = frm.get_next_boards();
    board_lookup = get_player_board_set(moves, PAWN_OFFSET);
    GTEST_ASSERT_EQ(moves.size(), 6);
    GTEST_ASSERT_EQ(count_bits(frm.player.pawns), 2);
    strike_moves = 0;
    for(auto m: moves){
        GTEST_ASSERT_EQ(count_bits(m.player.pawns), 2);
        if(m.opponent.pawns){
            GTEST_ASSERT_EQ(count_bits(m.opponent.pawns), 1);
        }
        else{
            GTEST_ASSERT_GT(m.player.pawns & ((uint64_t) 1)<<18, 0);
             ++strike_moves;
        }
    }
    GTEST_ASSERT_EQ(strike_moves, 2);
}

TEST(bitboard, test_opponent_pawn_moves)
{
    bitboard_player_set bb;
    bitboard_player_set bbo(true);
    bbo.rooks = 0;
    bbo.knights = 0;
    bbo.bishops = 0;
    bbo.king = 0;
    bbo.queen = 0;
    bitboard_frame frm(bb,bbo);

    auto moves = frm.get_opponent_next_boards();
    GTEST_ASSERT_EQ(moves.size(), 16);
    GTEST_ASSERT_EQ(count_bits(frm.opponent.pawns), 8);

    //test two space basic moves
    frm.opponent.pawns = ((uint64_t) 1)<<54;
    moves = frm.get_opponent_next_boards();
    GTEST_ASSERT_EQ(moves.size(), 2);
    GTEST_ASSERT_EQ(count_bits(frm.opponent.pawns), 1);
    std::set<uint64_t> board_lookup {get_opp_board_set(moves, PAWN_OFFSET)};
    GTEST_ASSERT_NE(board_lookup.find(((uint64_t) 1)<<46), board_lookup.end());
    GTEST_ASSERT_NE(board_lookup.find(((uint64_t) 1)<<38), board_lookup.end());
    GTEST_ASSERT_EQ(board_lookup.find(((uint64_t) 1)<<45), board_lookup.end());
    GTEST_ASSERT_EQ(board_lookup.find(((uint64_t) 1)<<47), board_lookup.end());

    //test only one space available away from starting row
    frm.player.pawns = 0;
    frm.player.rooks = 0;
    frm.player.knights = 0;
    for(auto v : std::vector<size_t>{8,16,24,32}){
        frm.opponent.pawns = ((uint64_t) 1)<<v;
        moves = frm.get_opponent_next_boards();
        GTEST_ASSERT_EQ(moves.size(), 1);
        GTEST_ASSERT_EQ(count_bits(frm.opponent.pawns), 1);
        GTEST_ASSERT_EQ(count_bits(moves[0].opponent.pawns), 1);
        GTEST_ASSERT_EQ(moves[0].opponent.pawns, ((uint64_t) 1)<<(v-8));
    }
    //no moves at edge
    frm.opponent.pawns = ((uint64_t) 1)<<0;
    moves = frm.get_opponent_next_boards();
    GTEST_ASSERT_EQ(moves.size(), 0);

    //test no moves if blocked on front
    frm.player.pawns = ((uint64_t) 1)<<47;
    frm.opponent.pawns = ((uint64_t) 1)<<55;
    moves = frm.get_opponent_next_boards();
    GTEST_ASSERT_EQ(moves.size(), 0);
    GTEST_ASSERT_EQ(count_bits(frm.player.pawns), 1);
    GTEST_ASSERT_EQ(count_bits(frm.opponent.pawns), 1);

    //test move available if strikeable added
    frm.player.pawns |= ((uint64_t) 1)<<46;
    moves = frm.get_opponent_next_boards();
    GTEST_ASSERT_EQ(moves.size(), 1);
    GTEST_ASSERT_EQ(count_bits(frm.opponent.pawns), 1);
    GTEST_ASSERT_EQ(count_bits(frm.player.pawns), 2);
    GTEST_ASSERT_EQ(moves[0].opponent.pawns, ((uint64_t) 1)<<46);

    //test moves does not strike on same row
    frm.player.pawns = ((uint64_t) 1)<<48;
    moves = frm.get_opponent_next_boards();
    GTEST_ASSERT_EQ(moves.size(), 2);
    GTEST_ASSERT_EQ(count_bits(frm.opponent.pawns), 1);
    for(auto m: moves){
        GTEST_ASSERT_EQ(count_bits(m.player.pawns), 1);
        GTEST_ASSERT_EQ(count_bits(m.opponent.pawns), 1);
    }

    //test all three moves available when strike is present
    frm.player.pawns = ((uint64_t) 1)<<46;
    moves = frm.get_opponent_next_boards();
    GTEST_ASSERT_EQ(moves.size(), 3);
    GTEST_ASSERT_EQ(count_bits(frm.player.pawns), 1);
    size_t strike_moves = 0;
    for(auto m: moves){
        GTEST_ASSERT_EQ(count_bits(m.opponent.pawns), 1);
        if(m.player.pawns){
            GTEST_ASSERT_EQ(count_bits(m.player.pawns), 1);
        }
        else{
             GTEST_ASSERT_EQ(m.opponent.pawns, ((uint64_t) 1)<<46);
             ++strike_moves;
        }
    }
    GTEST_ASSERT_EQ(strike_moves, 1);

    //test strikeable 
    frm.player.pawns = ((uint64_t) 1)<<45;
    frm.opponent.pawns = ((uint64_t) 1)<<52;
    moves = frm.get_opponent_next_boards();
    GTEST_ASSERT_EQ(moves.size(), 3);
    GTEST_ASSERT_EQ(count_bits(frm.opponent.pawns), 1);
    strike_moves = 0;
    for(auto m: moves){
        GTEST_ASSERT_EQ(count_bits(m.opponent.pawns), 1);
        if(m.player.pawns){
            GTEST_ASSERT_EQ(count_bits(m.player.pawns), 1);
        }
        else{
             GTEST_ASSERT_GT(m.opponent.pawns & ((uint64_t) 1)<<45, 0);
             ++strike_moves;
        }
    }
    GTEST_ASSERT_EQ(strike_moves, 1);

    //test strikeable from two pawns
    frm.opponent.pawns |= ((uint64_t) 1)<<54;
    moves = frm.get_opponent_next_boards();
    GTEST_ASSERT_EQ(moves.size(), 6);
    GTEST_ASSERT_EQ(count_bits(frm.opponent.pawns), 2);
    strike_moves = 0;
    for(auto m: moves){
        GTEST_ASSERT_EQ(count_bits(m.opponent.pawns), 2);
        if(m.player.pawns){
            GTEST_ASSERT_EQ(count_bits(m.player.pawns), 1);
        }
        else{
            GTEST_ASSERT_GT(m.opponent.pawns & ((uint64_t) 1)<<45, 0);
             ++strike_moves;
        }
    }
    GTEST_ASSERT_EQ(strike_moves, 2);
}



int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}