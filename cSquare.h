//////////////////////////////////////////////////////////////////////////////////
// cSquare.h
// - represents an individual square in Falling Blocks
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Enums.h"
#include "Defines.h"

struct SDL_Surface;

class cSquare
{
private:
    // Location of the center of the square //
    int m_CenterX;
    int m_CenterY;

    // Type of block. Needed to locate the correct square in our bitmap //
    BlockType m_BlockType;

    // A pointer to our bitmap surface from "Main.cpp" //
    SDL_Surface* m_Bitmap;

public:
    // Default constructor, your compiler will probably require this //
    cSquare()
    {
    }

    // Main constructor takes location and type of block, //
    // and pointer to our bitmap surface. //
    cSquare(int x, int y, SDL_Surface* bitmap, BlockType type);

    // Draw() takes a pointer to the surface to draw to (our window) //
    void Draw(SDL_Surface* window);

    // Remember, SQUARE_MEDIAN represents the distance from the square's center to //
    // its sides. SQUARE_MEDIAN*2 gives us the width and height of our squares.    //
    void Move(Direction dir);

    // Accessors //
    int GetCenterX() { return m_CenterX; }
    int GetCenterY() { return m_CenterY; }

    // Mutators //
    void SetCenterX(int x) { m_CenterX = x; }
    void SetCenterY(int y) { m_CenterY = y; }
};

//  Aaron Cox, 2004 //
