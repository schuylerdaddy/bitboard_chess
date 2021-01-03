#include "bitboard.h"

#include<iostream>

void print_board(const char * label, uint64_t board){
    std::cout<<label<<":\n";
    for(int i = 0; i <8; ++i){
        for(int j = 0; j <8; ++j){
            auto outchar = (board&1)?'1':'0';
            std::cout<< outchar;
            board>>=1;
        }     
        std::cout<<"\n";
    }
    std::cout<<std::endl;
}

void print_frame(const char * label, bitboard_frame& frame){
    std::cout<<label<<":\n";
    auto arr {frame.to_ascii_array()};
    size_t pos{0};
    for(int i = 0; i <8; i++){
        for(int j = 0; j <8; ++j){
            auto outchar = arr.data[pos];
            std::cout<< outchar;
            ++pos;
        }     
        std::cout<<"\n";
    }
    std::cout<<std::endl;
}

int main(int argc, char** argv){
    uint64_t val = 0;
    if(argc > 1){
        val = (uint64_t)atoll(argv[1]);
        print_board("Bit val", val);
        return 1;
    }

    

    bitboard_player_set bb;
    bitboard_player_set bbo(true);
    bb.pawns = 0;
    bb.barrier = (uint64_t)1<<18 | (uint64_t)1<<34 | (uint64_t)1<<27;
    bb.rooks = 1<<26 | 1 <<29;
    bb.knights = 0;
    bb.bishops = 0;
    bb.king = 0;
    bb.queen = 0;
    print_board("Player Rook", bb.rooks); 

   bitboard_frame frm(bb,bbo);
   int i =1;
   print_frame("Start", frm);
   for(auto& frame : frm.get_next_boards()){
       auto label {"successor " + std::to_string(i)};
       print_frame(label.c_str(), frame);
       ++i;
   }

//     i=1;
//     frm.opponent.pawns = ((uint64_t) 1)<<15;
//    print_frame("Start", frm);
//    for(auto& frame : frm.get_next_boards()){
//        auto label {"successor " + std::to_string(i)};
//        print_frame(label.c_str(), frame);
//        ++i;
//    }


//    print_board("Player Rooks", bb.rooks);
//    print_board("Player Knights", bb.knights);
//    print_board("Player Bishops", bb.bishops);
//    print_board("Player Queen", bb.queen);
//    print_board("Player King", bb.king);

//    print_board("Opponent Pawns", bbo.pawns); 
//    print_board("Opponent Rooks",bbo.rooks);
//    print_board("Opponent Knights",bbo.knights);
//    print_board("Opponent Bishops",bbo.bishops);
//    print_board("Opponent Queen",bbo.queen);
//    print_board("Opponent King",bbo.king);

//    print_board("Player Board", bb.full_player_board());
//    print_board("Opponent Board", bbo.full_player_board());

    // bitboard_frame frame(bb,bbo);
    // print_frame("Initial Frame", frame);

    //  print_board("Left", MASK_OFF_LEFT_DOUBLE);
    //  print_board("Right", MASK_OFF_RIGHT_DOUBLE);
    //  print_board("Top", MASK_OFF_TOP_DOUBLE);
    //  print_board("Bottom", MASK_OFF_BOTTOM_DOUBLE);
}