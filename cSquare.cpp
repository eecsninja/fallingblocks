//////////////////////////////////////////////////////////////////////////////////
// cSquare.cpp
// - Implementation of the square class functions.
//////////////////////////////////////////////////////////////////////////////////

#include "cSquare.h"

#include <SDL/SDL.h>

// Main constructor takes location and type of block, //
// and pointer to our bitmap surface. //
cSquare::cSquare(int x, int y, SDL_Surface* bitmap, BlockType type)
        : m_CenterX(x), m_CenterY(y), m_Bitmap(bitmap), m_BlockType(type)
{
}

// Draw() takes a pointer to the surface to draw to (our window) //
void cSquare::Draw(SDL_Surface* window) const
{
    SDL_Rect source;

    // The bitmap of each color of square is arranged in the same order as the
    // block type enums.
    source.w = SQUARE_SIZE;
    source.h = SQUARE_SIZE;
    source.x = 0;
    source.y = m_BlockType * source.h;

    // Draw at square's current location. Remember that m_X and m_Y store the center of the square. //
    SDL_Rect destination = { m_CenterX - SQUARE_MEDIAN, m_CenterY - SQUARE_MEDIAN,
        SQUARE_SIZE, SQUARE_SIZE };

    SDL_BlitSurface(m_Bitmap, &source, window, &destination);
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
