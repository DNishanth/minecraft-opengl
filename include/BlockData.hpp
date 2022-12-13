#ifndef BLOCKDATA_HPP
#define BLOCKDATA_HPP

#include "Transform.hpp"

#define WIDTH 32
#define HEIGHT 32
#define DEPTH 32

struct BlockData {
    bool isVisible;
    int blockType;
    Transform m_transform; // Store transformations
};

struct BlocksArray {
    BlockData blocks[WIDTH][HEIGHT][DEPTH];

    BlockData& getBlock(int x, int y, int z) {
        return blocks[x][y][z];
    }
};

#endif