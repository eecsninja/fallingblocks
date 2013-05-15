//////////////////////////////////////////////////////////////////////////////////
// LandedSquares.h
// - A container to handle the squares that have landed.
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "cSquare.h"

#include "Defines.h"

class Screen;

class LandedSquares {
  private:
    // Use this struct to keep track of squares that have landed.  This way,
    // there is no need to update the coordinates of all squares above a line
    // that was cleared.
    struct LandedSquare {
        int type;           // Type of the block that produced this square.
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

  public:
    LandedSquares();

    void Init();

    // Draw the squares to |screen|.
    void Draw(Screen* screen) const;

    // Iterate through each square of each row, checking for collisions.
    bool CheckCollision(int square_x, int square_y, int distance) const;

    // Return number of lines cleared or zero if no lines were cleared.
    int CheckCompletedLines();

    // Add a square that has landed.
    void Add(const cSquare& square);

    // Clear all landed squares.
    void Clear();
};

//  Aaron Cox, 2004 //
//  Simon Que, 2013 //
