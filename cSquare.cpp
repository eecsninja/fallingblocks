//////////////////////////////////////////////////////////////////////////////////
// cSquare.cpp
// - Implementation of the square class functions.
//////////////////////////////////////////////////////////////////////////////////

#include "cSquare.h"

#include "Screen.h"

// Main constructor takes location and type of block, //
// and pointer to our bitmap surface. //
cSquare::cSquare(int x, int y, int type)
        : m_CenterX(x), m_CenterY(y), m_BlockType(type)
{
}

// Draw() takes a pointer to the surface to draw to (our window) //
void cSquare::Draw(Screen* screen) const
{
    // Draw at square's current location. Remember that m_X and m_Y store the center of the square. //
    screen->DrawSquare(GetX(), GetY(), SQUARE_SIZE, SQUARE_SIZE, m_BlockType);
}

// Remember, SQUARE_MEDIAN represents the distance from the square's center to //
// its sides. SQUARE_MEDIAN*2 gives us the width and height of our squares.    //
void cSquare::Move(Direction dir)
{
    switch (dir)
    {
    case LEFT:
        m_CenterX -= SQUARE_SIZE;
        break;
    case RIGHT:
        m_CenterX += SQUARE_SIZE;
        break;
    case DOWN:
        m_CenterY += SQUARE_SIZE;
        break;
    }
}

//  Aaron Cox, 2004 //
//  Simon Que, 2013 //
