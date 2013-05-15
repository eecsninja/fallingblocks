//////////////////////////////////////////////////////////////////////////////////
// cSquare.h
// - represents an individual square in Falling Blocks
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Enums.h"
#include "Defines.h"

class cSquare
{
private:
    // Location of the top left of the square.
    int m_X;
    int m_Y;

    // Type of block. Needed to locate the correct square in our bitmap //
    int m_BlockType;

public:
    // Default constructor, your compiler will probably require this //
    cSquare()
    {
    }

    // Main constructor takes center location and type of block.
    cSquare(int center_x, int center_y, int type);

    // Remember, SQUARE_MEDIAN represents the distance from the square's center to //
    // its sides. SQUARE_MEDIAN*2 gives us the width and height of our squares.    //
    void Move(Direction dir);

    // Accessors //
    int GetX() const { return m_X; }
    int GetY() const { return m_Y; }
    int GetGridX() const { return m_X / SQUARE_SIZE; }
    int GetGridY() const { return m_Y / SQUARE_SIZE; }
    int GetType() const { return m_BlockType; }

    // Mutators //
    void SetX(int x) { m_X = x; }
    void SetY(int y) { m_Y = y; }
};

//  Aaron Cox, 2004 //
