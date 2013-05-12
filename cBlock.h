//////////////////////////////////////////////////////////////////////////////////
// cBlock.h
// - represents a block in Falling Blocks
// - this class is just four cSquare's placed in a specific shape
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "cSquare.h"

struct SDL_Surface;

class cBlock
{
private:
    // Location of the center of the block //
    int m_CenterX;
    int m_CenterY;

    // Type of block //
    BlockType m_Type;

    // Array of squares that make up the block //
    cSquare m_Squares[4];

public:
    // The constructor just sets the block location and calls SetupSquares //
    cBlock(int x, int y, SDL_Surface* bitmap, BlockType type);

    // Setup our block according to its location and type. Note that the squares //
    // are defined according to their distance from the block's center. This     //
    // function takes a surface that gets passed to cSquare's constructor.       //
    void SetupSquares(int x, int y, SDL_Surface* bitmap);

    // Draw() simply iterates through the squares and calls their Draw() functions. //
    void Draw(SDL_Surface* Window);

    // Move() simply changes the block's center and calls the squares' move functions. //
    void Move(Direction dir);

    // This function is explained in the tutorial. //
    void Rotate();

    // This function gets the locations of the squares after //
    // a rotation and returns an array of those values.      //
    int* GetRotatedSquares();

    // This returns a pointer to the array squares of the block. //
    cSquare* GetSquares();
};

//  Aaron Cox, 2004 //
