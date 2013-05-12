//////////////////////////////////////////////////////////////////////////////////
// LandedSquares.cpp
// - Implements functions for class LandedSquares.
//////////////////////////////////////////////////////////////////////////////////

#include "LandedSquares.h"

#include "math.h"    // We'll be using the abs() function located in "math.h"

#include "Defines.h"

// Draw the squares to |window|.
void LandedSquares::Draw(SDL_Surface* window) const {
    for (int i=0; i < squares.size(); i++)
        squares[i].Draw(window);
}

// Iterate through the squares vector, checking for collisions //
bool LandedSquares::CheckCollision(int center_x, int center_y,
                                   int distance) const {
    for (int i = 0; i < squares.size(); ++i) {
        if ((abs(center_x - squares[i].GetCenterX()) < distance) &&
            (abs(center_y - squares[i].GetCenterY()) < distance)) {
            return true;
        }
    }
    return false;
}

int LandedSquares::CheckCompletedLines() {
    // Store the amount of squares in each row in an array //
    int squares_per_row[MAX_NUM_LINES];

    // The compiler will fill the array with junk values if we don't do this //
    for (int index = 0; index < MAX_NUM_LINES; index++)
        squares_per_row[index] = 0;

    int row_size   = SQUARE_MEDIAN * 2;                // pixel size of one row
    int bottom     = GAME_AREA_BOTTOM - SQUARE_MEDIAN; // center of bottom row
    int top        = bottom - 12 * row_size;           // center of top row

    int num_lines = 0; // number of lines cleared
    int row;           // multipurpose variable


    // Check for full lines //
    for (int i = 0; i < squares.size(); ++i) {
        // Get the row the current square is in //
        row = (squares[i].GetCenterY() - top) / row_size;

        // Increment the appropriate row counter //
        squares_per_row[row]++;
    }

    // Erase any full lines //
    for (int line = 0; line < MAX_NUM_LINES; ++line) {
        // Check for completed lines //
        if (squares_per_row[line] == SQUARES_PER_ROW) {
            // Keep track of how many lines have been completed //
            num_lines++;

            // Find any squares in current row and remove them //
            for (int index = 0; index < squares.size(); ++index) {
                if ( ( (squares[index].GetCenterY() - top) / row_size ) == line )
                {
                    squares.erase(squares.begin() + index); // remove it from the vector
                    index--; // make sure we don't skip anything
                }
            }
        }
    }

    // Move squares above cleared line down //
    for (int index = 0; index < squares.size(); ++index) {
        for (int line = 0; line < MAX_NUM_LINES; ++line) {
            // Determine if this row was filled //
            if (squares_per_row[line] == SQUARES_PER_ROW)
            {
                // If it was, get the location of it within the game area //
                row = (squares[index].GetCenterY() - top) / row_size;

                // Now move any squares above that row down one //
                if ( row < line )
                {
                    squares[index].Move(DOWN);
                }
            }
        }
    }

    return num_lines;
}

//  Aaron Cox, 2004 //
//  Simon Que, 2013 //
