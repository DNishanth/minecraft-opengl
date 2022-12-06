#ifndef BLOCKDATA_HPP
#define BLOCKDATA_HPP

#include "Transform.hpp"

struct BlockData {
    bool isVisible;
    int blockType;
    Transform m_transform; // Store transformations
};

struct BlocksArray {
    BlockData blocks[16][16][16];

    BlockData& getBlock(int x, int y, int z) {
        return blocks[x][y][z];
    } 
};

#endif