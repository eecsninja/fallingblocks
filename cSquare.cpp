//////////////////////////////////////////////////////////////////////////////////
// cSquare.cpp
// - Implementation of the square class functions.
//////////////////////////////////////////////////////////////////////////////////

#include "cSquare.h"

// Main constructor takes location and type of block, //
// and pointer to our bitmap surface. //
cSquare::cSquare(int center_x, int center_y, int type) :
        m_X(center_x - SQUARE_MEDIAN),
        m_Y(center_y - SQUARE_MEDIAN),
        m_BlockType(type)
{
}

// Remember, SQUARE_MEDIAN represents the distance from the square's center to //
// its sides. SQUARE_MEDIAN*2 gives us the width and height of our squares.    //
void cSquare::Move(Direction dir)
{
    switch (dir)
    {
    case LEFT:
        m_X -= SQUARE_SIZE;
        break;
    case RIGHT:
        m_X += SQUARE_SIZE;
        break;
    case DOWN:
        m_Y += SQUARE_SIZE;
        break;
    }
}

//  Aaron Cox, 2004 //
//  Simon Que, 2013 //
