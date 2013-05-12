//////////////////////////////////////////////////////////////////////////////////
// Defines.h
//////////////////////////////////////////////////////////////////////////////////

#pragma once

// Window related defines //
#define WINDOW_WIDTH   300
#define WINDOW_HEIGHT  400
#define WINDOW_CAPTION "Falling Blocks"

// Game related defines //
#define FRAMES_PER_SECOND 30
#define FRAME_RATE        1000/FRAMES_PER_SECOND

// The game area is the area where the focus block can go. //
// These values are used to check collisions between the   //
// game squares and the sides of the game area.            //
#define GAME_AREA_LEFT   53
#define GAME_AREA_RIGHT  251
#define GAME_AREA_BOTTOM 298

#define NUM_LEVELS       5    // number of levels in the game
#define POINTS_PER_LINE  525  // points player receives for completing a line
#define POINTS_PER_LEVEL 6300 // points player needs to advance a level

#define INITIAL_SPEED    60  // initial interval at which focus block moves down
#define SPEED_CHANGE     10  // the above interval is reduced by this much each level

// Amount of time player has to move the focus block when its bottom collides with something. //
// Measured in number of frames. At 30 fps, 15 frames will give the player half a second.     //
#define SLIDE_TIME       15

#define SQUARES_PER_ROW  10  // number of squares that fit in a row
#define SQUARE_MEDIAN    10  // distance from the center of a square to its sides

// Starting position of the focus block //
#define BLOCK_START_X    151
#define BLOCK_START_Y    59

// Location on game screen for displaying... //
#define LEVEL_RECT_X        42  // current level
#define LEVEL_RECT_Y        320
#define SCORE_RECT_X         42  // current score
#define SCORE_RECT_Y        340
#define NEEDED_SCORE_RECT_X 42  // score needed for next level
#define NEEDED_SCORE_RECT_Y 360
#define NEXT_BLOCK_CIRCLE_X 214 // next block in line to be focus block
#define NEXT_BLOCK_CIRCLE_Y 347

// Locations within bitmap of background screens //
#define LEVEL_ONE_X   0
#define LEVEL_ONE_Y   0
#define LEVEL_TWO_X   300
#define LEVEL_TWO_Y   0
#define LEVEL_THREE_X 300
#define LEVEL_THREE_Y 0
#define LEVEL_FOUR_X  0
#define LEVEL_FOUR_Y  396
#define LEVEL_FIVE_X  300
#define LEVEL_FIVE_Y  396

// Location within bitmap of colored squares //
#define RED_SQUARE_X    600
#define RED_SQUARE_Y    400
#define PURPLE_SQUARE_X 620
#define PURPLE_SQUARE_Y 400
#define GREY_SQUARE_X   640
#define GREY_SQUARE_Y   400
#define BLUE_SQUARE_X   660
#define BLUE_SQUARE_Y   400
#define GREEN_SQUARE_X  680
#define GREEN_SQUARE_Y  400
#define BLACK_SQUARE_X  700
#define BLACK_SQUARE_Y  400
#define YELLOW_SQUARE_X 720
#define YELLOW_SQUARE_Y 400

//  Aaron Cox, 2004 //
