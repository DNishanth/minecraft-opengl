#ifndef BLOCKDATA_HPP
#define BLOCKDATA_HPP

#include "Transform.hpp"
#include <iostream>

#define WIDTH 100
#define HEIGHT 256
#define DEPTH 100

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
    Snow,
    Empty
};

struct BlockData {
    bool isVisible;
    int blockType;
    Transform m_transform; // Store transformations
};

struct BlocksArray {
    BlockData* blocks;

    // Allocate memory for blocks and initialize structs
    BlocksArray() {
        blocks = new BlockData[WIDTH * HEIGHT * DEPTH];
        for (int x = 0; x < WIDTH; x++) {
            for (int y = 0; y < HEIGHT; y++) {
                for (int z = 0; z < DEPTH; z++) {
                    getBlock(x, y, z).isVisible = false;
                    getBlock(x, y, z).blockType = Empty;
                    getBlock(x, y, z).m_transform.Translate(x, y, z);
                }
            }
        }
    }

    ~BlocksArray() {
        delete[] blocks;
    }

    // Return true if the block coordinates are within the WIDTH, HEIGHT, and DEPTH bounds
    bool isValidBlock(int x, int y, int z) {
        return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT && z >= 0 && z < DEPTH;
    }

    BlockData& getBlock(int x, int y, int z) {
        return blocks[z + y*DEPTH + x*HEIGHT*DEPTH];
    }

    // TODO: rename to isSolidBlock?
    bool isCoveredBlock(int x, int y, int z) {
        return isValidBlock(x, y, z) && getBlock(x, y, z).blockType != Empty;
    }

    bool isSurrounded(int x, int y, int z) {
        return isCoveredBlock(x - 1, y, z) && isCoveredBlock(x + 1, y, z) &&
                isCoveredBlock(x, y - 1, z) && isCoveredBlock(x, y + 1, z) &&
                isCoveredBlock(x, y, z - 1) && isCoveredBlock(x, y, z + 1);
    }

    void hideBlockIfSurrounded(int x, int y, int z) {
        if (isValidBlock(x, y, z) && isSurrounded(x, y, z)) {
            getBlock(x, y, z).isVisible = false;
        }
    }

    void revealSurroundingBlocks(int x, int y, int z) {
        makeVisible(x - 1, y, z);
        makeVisible(x + 1, y, z);
        makeVisible(x, y - 1, z);
        makeVisible(x, y + 1, z);
        makeVisible(x, y, z - 1);
        makeVisible(x, y, z + 1);
    }

    void hideSurroundingBlocks(int x, int y, int z) {
        hideBlockIfSurrounded(x - 1, y, z);
        hideBlockIfSurrounded(x + 1, y, z);
        hideBlockIfSurrounded(x, y - 1, z);
        hideBlockIfSurrounded(x, y + 1, z);
        hideBlockIfSurrounded(x, y, z - 1);
        hideBlockIfSurrounded(x, y, z + 1);
    }

    // TODO: use isCoveredBlock
    void makeVisible(int x, int y, int z) {
        if (isValidBlock(x, y, z) && getBlock(x, y, z).blockType != Empty) {
            getBlock(x, y, z).isVisible = true;
        }
    }

    void makeInvisible(int x, int y, int z) {
        if (isValidBlock(x, y, z) && getBlock(x, y, z).blockType != Empty) {
            getBlock(x, y, z).isVisible = false;
        }
    }
};

#endif