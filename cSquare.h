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
    void Draw(SDL_Surface* window) const;

    // Remember, SQUARE_MEDIAN represents the distance from the square's center to //
    // its sides. SQUARE_MEDIAN*2 gives us the width and height of our squares.    //
    void Move(Direction dir);

    // Accessors //
    int GetX() const { return m_CenterX - SQUARE_MEDIAN; }
    int GetY() const { return m_CenterY - SQUARE_MEDIAN; }
    int GetCenterX() const { return m_CenterX; }
    int GetCenterY() const { return m_CenterY; }
    BlockType GetType() const { return m_BlockType; }

    // Mutators //
    void SetX(int x) { m_CenterX = x + SQUARE_MEDIAN; }
    void SetY(int y) { m_CenterY = y + SQUARE_MEDIAN; }
    void SetCenterX(int x) { m_CenterX = x; }
    void SetCenterY(int y) { m_CenterY = y; }
};

//  Aaron Cox, 2004 //
