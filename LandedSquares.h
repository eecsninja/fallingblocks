//////////////////////////////////////////////////////////////////////////////////
// LandedSquares.h
// - A container to handle the squares that have landed.
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

#include "cSquare.h"

struct SDL_Surface;

class LandedSquares {
  private:
    std::vector<cSquare> squares;

  public:
    LandedSquares() {}

    // Draw the squares to |window|.
    void Draw(SDL_Surface* window) const;

    // Iterate through the squares vector, checking for collisions.
    bool CheckCollision(int center_x, int center_y, int distance) const;

    // Return number of lines cleared or zero if no lines were cleared.
    int CheckCompletedLines();

    // Add a square that has landed.
    void Add(const cSquare& square) {
        squares.push_back(square);
    }

    // Clear all landed squares.
    void Clear() {
        squares.clear();
    }
};

//  Aaron Cox, 2004 //
//  Simon Que, 2013 //
