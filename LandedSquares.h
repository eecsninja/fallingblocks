//////////////////////////////////////////////////////////////////////////////////
// LandedSquares.h
// - A container to handle the squares that have landed.
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "cSquare.h"

struct SDL_Surface;

#include "Defines.h"

class LandedSquares {
  private:
    // Use this struct to keep track of squares that have landed.  This way,
    // there is no need to update the coordinates of all squares above a line
    // that was cleared.
    struct LandedSquare {
        BlockType type;     // Type of the block that produced this square.
        bool valid:1;         // If this flag is not set, then the struct acts as
                            // a placeholder in an array.
        LandedSquare() : valid(false) {}
    };

    // One row of the game field.
    struct SquareRow {
        LandedSquare squares[SQUARES_PER_ROW];
        bool empty:1;    // Indicates that this row doesn't contain any squares.
        SquareRow() : empty(true) {}
        void Clear();
    };

    // All the rows that fill up the game area.  These are not necessarily in
    // physical order.  It acts as a cache of row objects.
    SquareRow rows[MAX_NUM_LINES];

    // All the rows in physical order.  These point to the objects in |rows|.
    SquareRow* row_ptrs[MAX_NUM_LINES];

    SDL_Surface* bitmap;

  public:
    LandedSquares();

    void Init(SDL_Surface* bitmap);

    // Draw the squares to |window|.
    void Draw(SDL_Surface* window) const;

    // Iterate through each square of each row, checking for collisions.
    bool CheckCollision(int center_x, int center_y, int distance) const;

    // Return number of lines cleared or zero if no lines were cleared.
    int CheckCompletedLines();

    // Add a square that has landed.
    void Add(const cSquare& square);

    // Clear all landed squares.
    void Clear();
};

//  Aaron Cox, 2004 //
//  Simon Que, 2013 //
