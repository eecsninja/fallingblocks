//////////////////////////////////////////////////////////////////////////////////
// cBlock.h
// - represents a block in Falling Blocks
// - this class is just four cSquare's placed in a specific shape
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "cSquare.h"

#define CBLOCK_NUM_SQUARES             4

class Screen;

class cBlock
{
private:
    // Location of the center of the block //
    int m_CenterX;
    int m_CenterY;

    // Type of block //
    BlockType m_Type;

    // Array of squares that make up the block //
    cSquare m_Squares[CBLOCK_NUM_SQUARES];

public:
    // Default constructor.
    cBlock() {}

    // The constructor just sets the block location and calls SetupSquares //
    cBlock(int x, int y, BlockType type);

    // Setup our block according to its location and type. Note that the
    // squares are defined according to their distance from the block's center.
    void SetupSquares(int x, int y);

    // Draw() simply iterates through the squares and calls their Draw() functions. //
    void Draw(Screen* screen) const;

    // Move() simply changes the block's center and calls the squares' move functions. //
    void Move(Direction dir);

    // This function is explained in the tutorial. //
    void Rotate();

    // This function gets the locations of the squares after //
    // a rotation and stores those values in |array|.        //
    void GetRotatedSquares(int *array) const;

    // This returns a pointer to the array squares of the block. //
    const cSquare* GetSquares() const;
};

//  Aaron Cox, 2004 //
