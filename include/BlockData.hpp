#ifndef BLOCKDATA_HPP
#define BLOCKDATA_HPP

#include "Transform.hpp"
#include <iostream>

#define WIDTH 10
#define HEIGHT 100
#define DEPTH 10

enum BlockType {
    Dirt,
    Grass,
    Plank,
    Brick,
    Cobblestone,
    Sandstone,
    Mossystone,
    LightBlueWool,
    OrangeWool,
    Empty
};
#define NUM_BLOCK_TYPES 3

struct BlockData {
    bool isVisible;
    int blockType;
    Transform m_transform; // Store transformations
};

struct BlocksArray {
    BlockData* blocks;

    BlocksArray() {
        blocks = new BlockData[WIDTH * HEIGHT * DEPTH];
    }

    ~BlocksArray() {
        std::cout << "Freeing BlockArray" << std::endl;
        delete[] blocks;
    }

    bool isValidBlock(int x, int y, int z) {
        return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT && z >= 0 && z < DEPTH;
    }

    BlockData& getBlock(int x, int y, int z) {
        return blocks[z + y*DEPTH + x*HEIGHT*DEPTH];
    }

    bool isCoveredBlock(int x, int y, int z) {
        return isValidBlock(x, y, z) && getBlock(x, y, z).blockType != Empty;
    }
};

#endif