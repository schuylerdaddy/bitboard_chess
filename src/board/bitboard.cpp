#include "bitboard.h"

#include <iostream>

bitboard_player_set::bitboard_player_set(bool opponent){
    auto pawn_row = opponent? 6 : 1;
    auto main_row = opponent? 7 : 0;

    this->pawns = start_pawns(pawn_row);
    this->king = start_king(main_row);
    this->queen = start_queen(main_row);
    this->bishops = start_bishop(main_row);
    this->knights = start_knight(main_row);
    this->rooks = start_rook(main_row);
    #ifdef UNITTEST
    this->barrier= start_rook(main_row);
    #endif
}

uint64_t bitboard_player_set::full_player_board(){
#ifdef UNITTEST
    return this->pawns | this->rooks | this->bishops | this->knights | this->king | this->queen | this->barrier;
#endif
    return this->pawns | this->rooks | this->bishops | this->knights | this->king | this->queen;
}

void bitboard_player_set::remove_piece(size_t struct_offset, size_t absolute_position){
    auto piece {reinterpret_cast<uint64_t*>(this) + struct_offset};
    *piece &= ~((uint64_t)(1)<<absolute_position);
}

void bitboard_player_set::remove_pieces(size_t absolute_position){
    pawns &= ~((uint64_t)(1)<<absolute_position);
    rooks &= ~((uint64_t)(1)<<absolute_position);
    bishops &= ~((uint64_t)(1)<<absolute_position);
    knights &= ~((uint64_t)(1)<<absolute_position);
    queen &= ~((uint64_t)(1)<<absolute_position);
}

void bitboard_player_set::move_piece(size_t struct_offset, size_t from_position, size_t to_position){
    auto piece {reinterpret_cast<uint64_t*>(this) + struct_offset};
    *piece &= ~((uint64_t)(1)<<from_position);
    *piece |= (uint64_t)(1)<<to_position;
}

bitboard_frame::bitboard_frame(bitboard_player_set player, bitboard_player_set opponent):
player{player}, opponent{opponent} {}

ascii_array bitboard_frame::to_ascii_array(){
    ascii_array arr;
    size_t pos{1};
    for(int i=0;i<64;++i){
        if(pos&player.pawns){
            arr.data[i] = 'P';
        }
        else if(pos&opponent.pawns){
            arr.data[i] = 'p';
        }
        else if(pos&player.rooks){
            arr.data[i] = 'R';
        }
        else if(pos&opponent.rooks){
            arr.data[i] = 'r';
        }
        else if(pos&player.knights){
            arr.data[i] = 'N';
        }
        else if(pos&opponent.knights){
            arr.data[i] = 'n';
        }
        else if(pos&player.bishops){
            arr.data[i] = 'B';
        }
        else if(pos&opponent.bishops){
            arr.data[i] = 'b';
        }
        else if(pos&player.queen){
            arr.data[i] = 'Q';
        }
        else if(pos&opponent.queen){
            arr.data[i] = 'q';
        }
        else if(pos&player.king){
            arr.data[i] = 'k';
        }
        else if(pos&opponent.king){
            arr.data[i] = 'k';
        }
        #ifdef UNITTEST
        else if(pos&player.barrier){
            arr.data[i] = '*';
        }
        #endif
        else{
            arr.data[i] = '.';
        }
        pos<<=1;
    }
    return arr;
}

bitboard_frame bitboard_frame::clone_from_player_move(size_t struct_offset, size_t from_position, size_t to_position){
    bitboard_frame moved_frame{*this};
    moved_frame.player.move_piece(struct_offset, from_position,to_position);
    return moved_frame;
}

bitboard_frame bitboard_frame::clone_from_opponent_move(size_t struct_offset, size_t from_position, size_t to_position){
    bitboard_frame moved_frame{*this};
    moved_frame.opponent.move_piece(struct_offset, from_position,to_position);
    return moved_frame;
}

std::vector<bitboard_frame> bitboard_frame::get_next_boards(){
    std::vector<bitboard_frame> next_boards;
    auto self_board{player.full_player_board()};
    auto opp_board{opponent.full_player_board()};
    auto strike_move{~self_board&opp_board};
    auto nonstrike_move{~self_board&~opp_board};

    //pawns
    auto p_move1 {(player.pawns<<8)&nonstrike_move};
    auto  p_move2 {(player.pawns<<16)&nonstrike_move};
    auto p_strike_left {((player.pawns&MASK_OFF_LEFT)<<9)&strike_move};
    auto p_strike_right {((player.pawns&MASK_OFF_RIGHT)<<7)&strike_move};

    //rooks
    uint64_t rookmove_list[28];
    int rookmove_dist[28];
    size_t rookmove_count{0};
    int move_dist{0};
    auto cur_rooks{player.rooks};
    decltype(cur_rooks) potential_move{0};
    while(cur_rooks){
        cur_rooks<<=8;
        potential_move=cur_rooks&~self_board;
        move_dist +=8;
        if(potential_move){
            rookmove_dist[rookmove_count] = move_dist;
            rookmove_list[rookmove_count++] = potential_move;
        }
        cur_rooks&=nonstrike_move; //ensure strike moves end progression
    }

    cur_rooks = player.rooks;
    move_dist = 0;
    while(cur_rooks){
        cur_rooks>>=8;
        potential_move=cur_rooks&~self_board;
        move_dist -=8;
        if(potential_move){
            rookmove_dist[rookmove_count] = move_dist;
            rookmove_list[rookmove_count++] = potential_move;
        }
        cur_rooks&=nonstrike_move; //ensure strike moves end progression
    }

    cur_rooks = player.rooks;
    move_dist = 0;
    while(cur_rooks){
        cur_rooks<<=1;
        potential_move=cur_rooks&~self_board;
        move_dist +=1;
        if(potential_move){
            rookmove_dist[rookmove_count] = move_dist;
            rookmove_list[rookmove_count++] = potential_move;
        }
        cur_rooks&=nonstrike_move&MASK_OFF_LEFT; //ensure strike moves end progression
        
    }

    cur_rooks = player.rooks;
    move_dist = 0;
    while(cur_rooks){
        cur_rooks>>=1;
        potential_move=cur_rooks&~self_board;
        move_dist -=1;
        if(potential_move){
            rookmove_dist[rookmove_count] = move_dist;
            rookmove_list[rookmove_count++] = potential_move;
        }
        cur_rooks&=nonstrike_move&MASK_OFF_RIGHT; //ensure strike moves end progression
    }

    for(size_t i=0; i<64; ++i){
        uint64_t mask {(uint64_t)1<<i};
        if(mask & p_move1){
            next_boards.emplace_back(clone_from_player_move(PAWN_OFFSET, i-8, i));
            if(mask<<8 & p_move2 && i <24){
                next_boards.emplace_back(clone_from_player_move(PAWN_OFFSET, i-8, i+8));
            }
        }
        if(mask & p_strike_left){
            next_boards.emplace_back(clone_from_player_move(PAWN_OFFSET,i-9,i));    
            next_boards.back().opponent.remove_pieces(i);
        }
        if(mask & p_strike_right){
            next_boards.emplace_back(clone_from_player_move(PAWN_OFFSET,i-7,i));    
            next_boards.back().opponent.remove_pieces(i);
        }
        for(size_t rook_idx=0;rook_idx<rookmove_count;++rook_idx){
            if(mask & rookmove_list[rook_idx]){
                next_boards.emplace_back(clone_from_player_move(ROOK_OFFSET,i-rookmove_dist[rook_idx],i));    
                next_boards.back().opponent.remove_pieces(i);
            }
        }
    }

    return next_boards;
}

std::vector<bitboard_frame> bitboard_frame::get_opponent_next_boards(){
   std::vector<bitboard_frame> next_boards;
    auto self_board{opponent.full_player_board()};
    auto opp_board{player.full_player_board()};
    auto strike_move{~self_board&opp_board};
    auto nonstrike_move{~self_board&~opp_board};

    //pawns
    auto p_move1 {(opponent.pawns>>8)&nonstrike_move};
    auto p_move2 {(opponent.pawns>>16)&nonstrike_move};
    auto p_strike_left {((opponent.pawns&MASK_OFF_RIGHT)>>9)&strike_move};
    auto p_strike_right {((opponent.pawns&MASK_OFF_LEFT)>>7)&strike_move};

    //rooks
    uint64_t rookmove_list[14];
    int rookmove_dist[14];
    size_t rookmove_count{0};
    int move_dist{0};
    auto cur_rooks{player.rooks};
    while(cur_rooks){
        cur_rooks<<=8;
        move_dist +=8;
        if(cur_rooks&nonstrike_move){
            rookmove_dist[rookmove_count] = move_dist;
            rookmove_list[rookmove_count++] = cur_rooks;
        }
        cur_rooks&=nonstrike_move; //ensure strike moves end progression
    }

    cur_rooks = player.rooks;
    rookmove_count=0;
    move_dist = 0;
    while(cur_rooks){
        cur_rooks>>=8;
        move_dist -=8;
        if(cur_rooks&nonstrike_move){
            rookmove_dist[rookmove_count] = move_dist;
            rookmove_list[rookmove_count++] = cur_rooks;
        }
        cur_rooks&=nonstrike_move; //ensure strike moves end progression
    }

    cur_rooks = player.rooks;
    rookmove_count=0;
    move_dist = 0;
    while(cur_rooks){
        cur_rooks<<=1;
        move_dist +=1;
        if(cur_rooks&nonstrike_move){
            rookmove_dist[rookmove_count] = move_dist;
            rookmove_list[rookmove_count++] = cur_rooks;
        }
        cur_rooks&=nonstrike_move; //ensure strike moves end progression
    }

    cur_rooks = player.rooks;
    rookmove_count=0;
    move_dist = 0;
    while(cur_rooks){
        cur_rooks>>=1;
        move_dist -=1;
        if(cur_rooks&nonstrike_move){
            rookmove_dist[rookmove_count] = move_dist;
            rookmove_list[rookmove_count++] = cur_rooks;
        }
        cur_rooks&=nonstrike_move; //ensure strike moves end progression
    }

    for(size_t i=0; i<64; ++i){
        uint64_t mask {(uint64_t)1<<i};

        //pawns
        if(mask & p_move1){
            next_boards.emplace_back(clone_from_opponent_move(PAWN_OFFSET, i+8, i));
            if(mask>>8 & p_move2 && i >39){
                next_boards.emplace_back(clone_from_opponent_move(PAWN_OFFSET, i+8, i-8));
            }
        }
        if(mask & p_strike_left){
            next_boards.emplace_back(clone_from_opponent_move(PAWN_OFFSET,i+9,i));    
            next_boards.back().player.remove_pieces(i);
        }
        if(mask & p_strike_right){
            next_boards.emplace_back(clone_from_opponent_move(PAWN_OFFSET,i+7,i));    
            next_boards.back().player.remove_pieces(i);
        }

        for(size_t rook_idx=0;rook_idx<rookmove_count;++rook_idx){
            if(mask & rookmove_list[rook_idx]){
                next_boards.emplace_back(clone_from_opponent_move(ROOK_OFFSET,i-rookmove_dist[rook_idx],i));    
                next_boards.back().player.remove_pieces(i);
            }
        }
    }

    return next_boards;
}