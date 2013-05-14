//////////////////////////////////////////////////////////////////////////////////
// cSquare.h
// - represents an individual square in Falling Blocks
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Enums.h"
#include "Defines.h"

class Screen;

class cSquare
{
private:
    // Location of the center of the square //
    int m_CenterX;
    int m_CenterY;

    // Type of block. Needed to locate the correct square in our bitmap //
    int m_BlockType;

public:
    // Default constructor, your compiler will probably require this //
    cSquare()
    {
    }

    // Main constructor takes location and type of block.
    cSquare(int x, int y, int type);

    // Draw() takes a pointer to the Screen controller.
    void Draw(Screen* screen) const;

    // Remember, SQUARE_MEDIAN represents the distance from the square's center to //
    // its sides. SQUARE_MEDIAN*2 gives us the width and height of our squares.    //
    void Move(Direction dir);

    // Accessors //
    int GetX() const { return m_CenterX - SQUARE_MEDIAN; }
    int GetY() const { return m_CenterY - SQUARE_MEDIAN; }
    int GetCenterX() const { return m_CenterX; }
    int GetCenterY() const { return m_CenterY; }
    int GetType() const { return m_BlockType; }

    // Mutators //
    void SetX(int x) { m_CenterX = x + SQUARE_MEDIAN; }
    void SetY(int y) { m_CenterY = y + SQUARE_MEDIAN; }
    void SetCenterX(int x) { m_CenterX = x; }
    void SetCenterY(int y) { m_CenterY = y; }
};

//  Aaron Cox, 2004 //
