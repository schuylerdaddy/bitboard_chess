#include<cstdint>
#include<cstddef> 
#include<cstring>

struct compressed_board{
    uint8_t data[32]; 
    compressed_board(uint8_t& data){
        std::memcpy(this->data, &data, sizeof(data));
    }
};