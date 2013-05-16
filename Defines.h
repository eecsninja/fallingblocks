//////////////////////////////////////////////////////////////////////////////////
// Defines.h
//////////////////////////////////////////////////////////////////////////////////

#pragma once

// Window related defines //
#define WINDOW_WIDTH   320
#define WINDOW_HEIGHT  240
#define WINDOW_CAPTION "Falling Blocks"

// Game related defines //
#define FRAMES_PER_SECOND 30
#define FRAME_RATE        1000/FRAMES_PER_SECOND

// The game area is the area where the focus block can go. //
// These values are used to check collisions between the   //
// game squares and the sides of the game area.            //
#define GAME_AREA_LEFT     2  // 16 / SQUARE_SIZE
#define GAME_AREA_RIGHT   12  // 192 / SQUARE_SIZE
#define GAME_AREA_TOP      1  // 16 / SQUARE_SIZE
#define GAME_AREA_BOTTOM  14  // 224 / SQUARE_SIZE

#define GAME_AREA_WIDTH   (GAME_AREA_RIGHT - GAME_AREA_LEFT)
#define GAME_AREA_HEIGHT  (GAME_AREA_BOTTOM - GAME_AREA_TOP)

#define NUM_LEVELS       5    // number of levels in the game
#define POINTS_PER_LINE  525  // points player receives for completing a line
#define POINTS_PER_LEVEL 6300 // points player needs to advance a level

#define INITIAL_SPEED    60  // initial interval at which focus block moves down
#define SPEED_CHANGE     10  // the above interval is reduced by this much each level

// Number of lines of squares before full.
#define MAX_NUM_LINES    GAME_AREA_HEIGHT

// Amount of time player has to move the focus block when its bottom collides with something. //
// Measured in number of frames. At 30 fps, 15 frames will give the player half a second.     //
#define SLIDE_TIME       15

#define SQUARES_PER_ROW  GAME_AREA_WIDTH  // number of squares that fit in a row
#define SQUARE_MEDIAN     8  // distance from the center of a square to its sides
#define SQUARE_SIZE    (SQUARE_MEDIAN * 2)   // Width and height of square.

// Text characters are 8x8.
#define TEXT_SIZE         8

// Starting position of the focus block //
#define BLOCK_START_X     6  // 96 / SQUARE_SIZE
#define BLOCK_START_Y     2  // 32 / SQUARE_SIZE

// Location on game screen for displaying... //
#define LEVEL_RECT_X         28  // current level (in text grid coordinates)
#define LEVEL_RECT_Y          4
#define SCORE_RECT_X         28  // current score (in text grid coordinates)
#define SCORE_RECT_Y          8
#define NEEDED_SCORE_RECT_X  28  // score needed for next level
#define NEEDED_SCORE_RECT_Y  13  //  (in text grid coordinates)

#define NEXT_BLOCK_CIRCLE_X  16  // next block in line to be focus block
#define NEXT_BLOCK_CIRCLE_Y  11  // (256 and 176) / SQUARE_SIZE

// Palette entry range used for color cycling.
#define COLOR_CYCLING_START_INDEX     8
#define COLOR_CYCLING_END_INDEX      15
#define NUM_CYCLED_COLORS    \
        (COLOR_CYCLING_END_INDEX - COLOR_CYCLING_START_INDEX + 1)

// Tile layer indexes.
#define BG_LAYER_INDEX                0
#define UI_LAYER_INDEX                1
#define TEXT_LAYER_INDEX              2
#define BLOCKS_LAYER_INDEX            3

// Palette indexes.
#define UI_PALETTE_INDEX              0
#define TEXT_PALETTE_INDEX            1
#define BLOCKS_PALETTE_INDEX          2
#define BG_PALETTE_INDEX              3

// Default tile color key and empty tile values.
#define DEFAULT_TILE_COLOR_KEY     0x00
#define DEFAULT_EMPTY_TILE_VALUE   0x00

// Colors of the font characters.
#define FONT_BLACK                    1
#define FONT_WHITE                    2

//  Aaron Cox, 2004 //
