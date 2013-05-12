//////////////////////////////////////////////////////////////////////////////////
// LandedSquares.cpp
// - Implements functions for class LandedSquares.
//////////////////////////////////////////////////////////////////////////////////

#include "LandedSquares.h"

#include <stdio.h>

#include "Defines.h"

#define abs(x)  ( ((x) >= 0) ? (x) : -(x) )

void LandedSquares::SquareRow::Clear() {
    for (int x = 0; x < SQUARES_PER_ROW; ++x)
        squares[x].valid = false;
    empty = true;
}

LandedSquares::LandedSquares() : bitmap(NULL) {
    Clear();
}

void LandedSquares::Init(SDL_Surface* bitmap) {
    this->bitmap = bitmap;
    Clear();
}

// Draw the squares to |window|.
void LandedSquares::Draw(SDL_Surface* window) const {
    for (int y = 0; y < MAX_NUM_LINES; ++y) {
        const SquareRow& row = *row_ptrs[y];
        if (row.empty)
            continue;
        for (int x = 0; x < SQUARES_PER_ROW; ++x) {
            const LandedSquare& square = row.squares[x];
            if (!square.valid)
                continue;
            cSquare temp_square(0, 0, bitmap, square.type);
            temp_square.SetX(GAME_AREA_LEFT + x * SQUARE_SIZE);
            temp_square.SetY(GAME_AREA_BOTTOM - (y + 1) * SQUARE_SIZE);
            temp_square.Draw(window);
        }
    }
}

// Iterate through each square of each row, checking for collisions.
bool LandedSquares::CheckCollision(int center_x, int center_y,
                                   int distance) const {
    for (int y = 0; y < MAX_NUM_LINES; ++y) {
        const SquareRow& row = *row_ptrs[y];
        if (row.empty)
            continue;
        for (int x = 0; x < SQUARES_PER_ROW; ++x) {
            const LandedSquare& square = row.squares[x];
            if (!square.valid)
                continue;
            cSquare temp_square(0, 0, bitmap, square.type);
            temp_square.SetX(GAME_AREA_LEFT + x * SQUARE_SIZE);
            temp_square.SetY(GAME_AREA_BOTTOM - (y + 1) * SQUARE_SIZE);
            int dx = center_x - temp_square.GetCenterX();
            int dy = center_y - temp_square.GetCenterY();
            if (abs(dx) < distance && abs(dy) < distance)
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

    int num_lines = 0; // number of lines cleared

    // Check for full lines //
    for (int y = 0; y < MAX_NUM_LINES; ++y) {
        const SquareRow& row = *row_ptrs[y];
        if (row.empty)
            continue;
        for (int x = 0; x < SQUARES_PER_ROW; ++x) {
            const LandedSquare& square = row.squares[x];
            if (!square.valid)
                continue;
            // Increment the appropriate row counter //
            ++squares_per_row[y];
        }
    }

    // Erase any full lines //
    for (int line = 0; line < MAX_NUM_LINES; ++line) {
        // Check for completed lines //
        if (squares_per_row[line] != SQUARES_PER_ROW)
            continue;
        // Keep track of how many lines have been completed //
        num_lines++;

        // Clear out the current row.
        SquareRow& row = *row_ptrs[line];
        for (int x = 0; x < SQUARES_PER_ROW; ++x)
            row.squares[x].valid = false;
        row.empty = true;

        // Move rows above this row down.
        for (int next_line = line + 1; next_line < MAX_NUM_LINES; ++next_line) {
            row_ptrs[next_line - 1] = row_ptrs[next_line];
            squares_per_row[next_line - 1] = squares_per_row[next_line];
        }

        // Push the empty row to the top.
        row_ptrs[MAX_NUM_LINES - 1] = &row;
        squares_per_row[MAX_NUM_LINES - 1] = 0;

        // Adjust the line counter by one because they were all shifted down.
        --line;
    }

    return num_lines;
}

// Add a square that has landed.
void LandedSquares::Add(const cSquare& square) {
    int x = (square.GetX() - GAME_AREA_LEFT) / SQUARE_SIZE;
    int y = (GAME_AREA_BOTTOM - square.GetY() - SQUARE_SIZE) / SQUARE_SIZE;
    row_ptrs[y]->squares[x].type = square.GetType();
    row_ptrs[y]->squares[x].valid = true;
    row_ptrs[y]->empty = false;
}

// Clear all landed squares.
void LandedSquares::Clear() {
    for (int y = 0; y < MAX_NUM_LINES; ++y) {
        rows[y].Clear();
        row_ptrs[y] = &rows[y];
    }
}

//  Aaron Cox, 2004 //
//  Simon Que, 2013 //
