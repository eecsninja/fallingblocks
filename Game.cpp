//////////////////////////////////////////////////////////////////////////////////
// Project: Falling Blocks (Tetris)
// File:    Game.cpp
//////////////////////////////////////////////////////////////////////////////////

#include "Game.h"

#include <stdlib.h>

#include "Defines.h" // Our defines header
#include "Enums.h"   // Our enums header
#include "System.h"          // Replaces SDL timer and input functions.

// These are used to enumerate the various game state functions.
enum GameStates {
    GAME_STATE_EXIT,
    GAME_STATE_MENU,
    GAME_STATE_GAME,
    GAME_STATE_WON,
    GAME_STATE_LOST,
};

// This function initializes our game //
void FallingBlocksGame::Init()
{
    m_Screen.Init();

    // Get the number of ticks since system was initialized //
    m_Timer = System::GetTicks();

    // Pass the squares bitmap to the landed squares container.
    m_OldSquares.Init();

    // Seed our random number generator //
    srand(System::GetTicks());

    // Initialize blocks and set them to their proper locations. //
    m_FocusBlock = cBlock(BLOCK_START_X * SQUARE_SIZE,
                          BLOCK_START_Y * SQUARE_SIZE, (rand() % 7 + 1));
    m_NextBlock  = cBlock(NEXT_BLOCK_CIRCLE_X * SQUARE_SIZE,
                          NEXT_BLOCK_CIRCLE_Y * SQUARE_SIZE, (rand() % 7 + 1));

    // We start by adding a pointer to our exit state, this way //
    // it will be the last thing the player sees of the game.   //
//    m_StateStack.push(GAME_STATE_EXIT);

    // Then we add a pointer to our menu state, this will //
    // be the first thing the player sees of our game.    //
//    m_StateStack.push(GAME_STATE_MENU);

    m_StateStack.push(GAME_STATE_GAME);
}

void FallingBlocksGame::MainLoop() {
    // Our game loop is just a while loop that breaks when our state stack is
    // empty.
    while (!m_StateStack.empty())
    {
        int state = m_StateStack.top();
        switch(state) {
        case GAME_STATE_EXIT:
            Exit();
            break;
        case GAME_STATE_MENU:
            Menu();
            break;
        case GAME_STATE_GAME:
            Game();
            break;
        case GAME_STATE_WON:
            GameWon();
            break;
        case GAME_STATE_LOST:
            GameLost();
            break;
        }
    }
}

// This function shuts down our game //
void FallingBlocksGame::Shutdown()
{
    // Clean up video resources.
    m_Screen.Cleanup();

    // Get pointers to the squares in our focus and next blocks so we can delete them. //
    // We must do this before we delete our blocks so we don't lose references to the squares. //
    // Note that these are pointers to arrays of pointers. //
    const cSquare* temp_array_1 = m_FocusBlock.GetSquares();
    const cSquare* temp_array_2 = m_NextBlock.GetSquares();
}

// This function handles the game's main menu. From here //
// the player can select to enter the game, or quit.     //
void FallingBlocksGame::Menu()
{
    // Here we compare the difference between the current time and the last time we //
    // handled a frame. If FRAME_RATE amount of time has passed, it's time for a new frame. //
    if ( (System::GetTicks() - m_Timer) >= FRAME_RATE )
    {
        // We start by calling our input function //
        HandleMenuInput();

        // Make sure nothing from the last frame is still drawn //
        ClearScreen();

        DisplayText("Start (G)ame", 8, 8, 12, 255, 255, 255, 0, 0, 0);
        DisplayText("(Q)uit Game",  8, 9, 12, 255, 255, 255, 0, 0, 0);

        // Update video screen.
        m_Screen.Update();

        // We've processed a frame so we now need to record the time at which we did it. //
        // This way we can compare this time with the next time our function gets called //
        // and see if enough time has passed. //
        m_Timer = System::GetTicks();
    }
}

// This function handles the main game. We'll control the   //
// drawing of the game as well as any necessary game logic. //
void FallingBlocksGame::Game()
{
    // Every frame we increase this value until it is equal to m_FocusBlockSpeed. //
    // When it reaches that value, we force the focus block down. //
    static int force_down_counter = 0;

    // Every frame, we check to see if the focus block's bottom has hit something. If it    //
    // has, we decrement this counter. If the counter hits zero, the focus block needs to   //
    // be changed. We use this counter so the player can slide the block before it changes. //
    static int slide_counter = SLIDE_TIME;

    // Here we compare the difference between the current time and the last time we //
    // handled a frame. If FRAME_RATE amount of time has, it's time for a new frame. //
    if ( (System::GetTicks() - m_Timer) >= FRAME_RATE )
    {
        HandleGameInput();

        force_down_counter++;

        if (force_down_counter >= m_FocusBlockSpeed)
        {
            // Always check for collisions before moving anything //
            if ( !CheckWallCollisions(m_FocusBlock, DOWN) && !CheckEntityCollisions(m_FocusBlock, DOWN) )
            {
                m_FocusBlock.Move(DOWN); // move the focus block
                force_down_counter = 0;   // reset our counter
            }
        }

        // Check to see if focus block's bottom has hit something. If it has, we decrement our counter. //
        if ( CheckWallCollisions(m_FocusBlock, DOWN) || CheckEntityCollisions(m_FocusBlock, DOWN) )
        {
            slide_counter--;
        }
        // If there isn't a collision, we reset our counter.    //
        // This is in case the player moves out of a collision. //
        else
        {
            slide_counter = SLIDE_TIME;
        }
        // If the counter hits zero, we reset it and call our //
        // function that handles changing the focus block.    //
        if (slide_counter == 0)
        {
            slide_counter = SLIDE_TIME;
            HandleBottomCollision();
        }

        m_Screen.WaitForNoVblank();  // Wait for vertical refresh if applicable.
        m_Screen.WaitForVblank();

        // Erase the squares.  They only need to be erased when the focus block
        // moves or the next block gets updated.  But we don't know when that
        // will happen (need to erase during blanking).  This doesn't take long.
        m_OldFocusBlock.Erase(&m_Screen);
        m_OldNextBlock.Erase(&m_Screen);

        // Make sure nothing from the last frame is still drawn. //
        ClearScreen();

        // Draw the background //
        DrawBackground();

        // Draw the focus block and next block. //
        m_FocusBlock.Draw(&m_Screen);
        m_NextBlock.Draw(&m_Screen);

        // Draw the old squares. //
        m_OldSquares.Draw(&m_Screen);

        // Draw the text for the current level, score, and needed score. //

        // We need to display the text ("Score:", "Level:", "Needed Score:") as well as the //
        // associated value. To do this, we use the string function append(). This function //
        // takes a char string so we call itoa() and store the char string in temp.         //
        char string[256];

        sprintf(string, "Level %u", m_Level);
        DisplayText(string, LEVEL_RECT_X, LEVEL_RECT_Y,
                    8, 0, 0, 0, 255, 255, 255);

        sprintf(string, "Score:");
        DisplayText(string, SCORE_RECT_X, SCORE_RECT_Y,
                    8, 0, 0, 0, 255, 255, 255);
        sprintf(string, "      %5u", m_Score);
        DisplayText(string, SCORE_RECT_X, SCORE_RECT_Y + 1,
                    8, 0, 0, 0, 255, 255, 255);

        sprintf(string, "Next level:");
        DisplayText(string, NEEDED_SCORE_RECT_X, NEEDED_SCORE_RECT_Y,
                    8, 0, 0, 0, 255, 255, 255);
        sprintf(string, "      %5u", m_Level*POINTS_PER_LEVEL);
        DisplayText(string, NEEDED_SCORE_RECT_X, NEEDED_SCORE_RECT_Y + 1,
                    8, 0, 0, 0, 255, 255, 255);

        // Update video screen.
        m_Screen.Update();

        // We've processed a frame so we now need to record the time at which we did it. //
        // This way we can compare this time the next time our function gets called and  //
        // see if enough time has passed between iterations. //
        m_Timer = System::GetTicks();
    }
}

// This function handles the game's exit screen. It will display //
// a message asking if the player really wants to quit.          //
void FallingBlocksGame::Exit()
{
    // Here we compare the difference between the current time and the last time we //
    // handled a frame. If FRAME_RATE amount of time has, it's time for a new frame. //
    if ( (System::GetTicks() - m_Timer) >= FRAME_RATE )
    {
        HandleExitInput();

        // Make sure nothing from the last frame is still drawn. //
        ClearScreen();

        DisplayText("Quit Game (Y or N)?", 6, 9, 12, 255, 255, 255, 0, 0, 0);

        // Update video screen.
        m_Screen.Update();

        // We've processed a frame so we now need to record the time at which we did it. //
        // This way we can compare this time the next time our function gets called and  //
        // see if enough time has passed between iterations. //
        m_Timer = System::GetTicks();
    }
}

// Display a victory message. //
void FallingBlocksGame::GameWon()
{
    if ( (System::GetTicks() - m_Timer) >= FRAME_RATE )
    {
        HandleWinLoseInput();

        ClearScreen();

        DisplayText("You Win!!!", 6, 8, 12, 255, 255, 255, 0, 0, 0);
        DisplayText("Quit Game (Y or N)?", 6, 9, 12, 255, 255, 255, 0, 0, 0);

        // Update video screen.
        m_Screen.Update();

        m_Timer = System::GetTicks();
    }
}

// Display a game over message. //
void FallingBlocksGame::GameLost()
{
    if ( (System::GetTicks() - m_Timer) >= FRAME_RATE )
    {
        HandleWinLoseInput();

        ClearScreen();

        DisplayText("You Lose.", 6, 8, 12, 255, 255, 255, 0, 0, 0);
        DisplayText("Quit Game (Y or N)?", 6, 9, 12, 255, 255, 255, 0, 0, 0);

        // Update video screen.
        m_Screen.Update();

        m_Timer = System::GetTicks();
    }
}

// This function draws the background //
void FallingBlocksGame::DrawBackground()
{
    m_Screen.DrawBackground(m_Level);
}

// This function simply clears the back buffer to black //
void FallingBlocksGame::ClearScreen()
{
    m_Screen.Clear();
}

// This function displays text to the screen. It takes the text //
// to be displayed, the location to display it, the size of the //
// text, and the color of the text and background.              //
void FallingBlocksGame::DisplayText(const char* text, int x, int y, int size, int fR, int fG, int fB, int bR, int bG, int bB)
{
    m_Screen.DisplayText(text, x, y, size, fR, fG, fB, bR, bG, bB);
}

// This function receives player input and //
// handles it for the game's menu screen.  //
void FallingBlocksGame::HandleMenuInput()
{
    // Get state of the input keys.
    System::KeyState key_state = System::GetKeyState();

    if (key_state.quit)
    {
        m_StateStack.pop();
        return;  // this state is done, exit the function
    }
    if (key_state.new_game)
    {
//      m_StateStack.push(GAME_STATE_GAME);
        return;  // this state is done, exit the function
    }
}

// This function receives player input and //
// handles it for the main game state.     //
void FallingBlocksGame::HandleGameInput()
{
    // Get state of the input keys.
    System::KeyState key_state = System::GetKeyState();

    if (key_state.quit)
    {
        m_StateStack.pop();
        return;  // this state is done, exit the function
    }

    if (key_state.up && !m_up_pressed)
    {
        // Check collisions before rotating.
        if (!CheckRotationCollisions(m_FocusBlock))
            m_FocusBlock.Rotate();
    }
    m_up_pressed = key_state.up;

    // For the left, right, and down arrow keys, we just set a bool variable.
    m_left_pressed = key_state.left;
    m_right_pressed = key_state.right;
    m_down_pressed = key_state.down;

    // Now we handle the arrow keys, making sure to check for collisions //
    if (m_down_pressed)
    {
        if ( !CheckWallCollisions(m_FocusBlock, DOWN) &&
             !CheckEntityCollisions(m_FocusBlock, DOWN) )
        {
            m_FocusBlock.Move(DOWN);
        }
    }
    if (m_left_pressed)
    {
        if ( !CheckWallCollisions(m_FocusBlock, LEFT) &&
             !CheckEntityCollisions(m_FocusBlock, LEFT) )
        {
            m_FocusBlock.Move(LEFT);
        }
    }
    if (m_right_pressed)
    {
        if ( !CheckWallCollisions(m_FocusBlock, RIGHT) &&
             !CheckEntityCollisions(m_FocusBlock, RIGHT) )
        {
            m_FocusBlock.Move(RIGHT);
        }
    }
}

// This function receives player input and //
// handles it for the game's exit screen.  //
void FallingBlocksGame::HandleExitInput()
{
    // Get state of the input keys.
    System::KeyState key_state = System::GetKeyState();

    if (key_state.quit)
    {
        m_StateStack.pop();
        return;  // this state is done, exit the function
    }

    // Yes //
    if (key_state.yes)
    {
        m_StateStack.pop();
        return;  // game is over, exit the function
    }
    // No //
    if (key_state.no)
    {
        m_StateStack.push(GAME_STATE_MENU);
        return;  // this state is done, exit the function
    }
}

// Input handling for win/lose screens. //
void FallingBlocksGame::HandleWinLoseInput()
{
    // Get state of the input keys.
    System::KeyState key_state = System::GetKeyState();

    if (key_state.quit) {
        m_StateStack.pop();
        return;
    }
    if (key_state.yes) {
        m_StateStack.pop();
        return;
    }

    if (key_state.no)
    {
        m_StateStack.pop();

//        m_StateStack.push(GAME_STATE_EXIT);
//        m_StateStack.push(GAME_STATE_MENU);
        return;
    }
}

// Check collisions between a given square and the squares in m_OldSquares //
bool FallingBlocksGame::CheckEntityCollisions(
        const cSquare& square, Direction dir) {
    // Width/height of a square. Also the distance //
    // between two squares if they've collided.    //
    int distance = SQUARE_SIZE;

    // Top left of the square.
    int x = square.GetX();
    int y = square.GetY();

    // Determine the location of the square after moving //
    switch (dir)
    {
    case DOWN:
        y += distance;
        break;

    case LEFT:
        x -= distance;
        break;

    case RIGHT:
        x += distance;
        break;
    }

    // Check collision against landed squares //
    if (m_OldSquares.CheckCollision(x, y, distance))
        return true;

    return false;
}

// Check collisions between a given block and the squares in m_OldSquares //
bool FallingBlocksGame::CheckEntityCollisions(
        const cBlock& block, Direction dir) {
    // Get an array of the squares that make up the given block //
    const cSquare* temp_array = block.GetSquares();

    // Now just call the other CheckEntityCollisions() on each square //
    for (int i = 0; i < CBLOCK_NUM_SQUARES; ++i)
    {
        if ( CheckEntityCollisions(temp_array[i], dir) )
            return true;
    }

    return false;
}

// Check collisions between a given square and the sides of the game area //
bool FallingBlocksGame::CheckWallCollisions(
        const cSquare& square, Direction dir) {
    // Get the location of the square
    int x = square.GetX();
    int y = square.GetY();

    // Get the location of the square after moving and see if its out of bounds //
    switch (dir)
    {
    case DOWN:
        return (y + SQUARE_SIZE * 2 > GAME_AREA_BOTTOM * SQUARE_SIZE);
    case LEFT:
        return (x - SQUARE_SIZE < GAME_AREA_LEFT * SQUARE_SIZE);
    case RIGHT:
        return (x + SQUARE_SIZE * 2 > GAME_AREA_RIGHT * SQUARE_SIZE);
    }

    return false;
}

// Check for collisions between a given block a the sides of the game area //
bool FallingBlocksGame::CheckWallCollisions(const cBlock& block, Direction dir)
{
    // Get an array of squares that make up the given block //
    const cSquare* temp_array = block.GetSquares();

    // Call other CheckWallCollisions() on each square //
    for (int i = 0; i < CBLOCK_NUM_SQUARES; ++i)
    {
        if ( CheckWallCollisions(temp_array[i], dir) )
            return true;
    }

    return false;
}

// Check for collisions when a block is rotated //
bool FallingBlocksGame::CheckRotationCollisions(const cBlock& block)
{
    // Get an array of values for the locations of the rotated block's squares //
    int rotated_squares[CBLOCK_NUM_SQUARES * 2];
    block.GetRotatedSquares(rotated_squares);

    // Distance between two touching squares //
    int distance = SQUARE_SIZE;

    for (int i = 0; i < CBLOCK_NUM_SQUARES; ++i)
    {
        // Check to see if the block will go out of bounds //
        if ( (rotated_squares[i*2] < GAME_AREA_LEFT * SQUARE_SIZE) ||
             (rotated_squares[i*2] + SQUARE_SIZE >
                    GAME_AREA_RIGHT * SQUARE_SIZE) ) {
            return true;
        }

        if (rotated_squares[i*2+1] + SQUARE_SIZE >
                    GAME_AREA_BOTTOM * SQUARE_SIZE ) {
            return true;
        }

        // Check to see if the block will collide with any squares //
        if (m_OldSquares.CheckCollision(rotated_squares[i*2],
                                        rotated_squares[i*2+1], distance)) {
            return true;
        }
    }

    return false;
}

// This function handles all of the events that   //
// occur when the focus block can no longer move. //
void FallingBlocksGame::HandleBottomCollision()
{
    ChangeFocusBlock();

    // Check for completed lines and store the number of lines completed //
    int num_lines = CheckCompletedLines();

    if ( num_lines > 0 )
    {
        // Increase player's score according to number of lines completed //
        m_Score += POINTS_PER_LINE * num_lines;

        // Check to see if it's time for a new level //
        if (m_Score >= m_Level * POINTS_PER_LEVEL)
        {
            m_Level++;
            CheckWin(); // check for a win after increasing the level
            m_FocusBlockSpeed -= SPEED_CHANGE; // shorten the focus blocks movement interval
        }
    }

    // Now would be a good time to check to see if the player has lost //
    CheckLoss();
}

// Add the squares of the focus block to m_OldSquares //
// and set the next block as the focus block. //
void FallingBlocksGame::ChangeFocusBlock()
{
    // Get an array of pointers to the focus block squares //
    const cSquare* square_array = m_FocusBlock.GetSquares();

    // Add focus block squares to m_OldSquares //
    for (int i = 0; i < CBLOCK_NUM_SQUARES; ++i)
        m_OldSquares.Add(square_array[i]);

    m_OldFocusBlock = m_FocusBlock;
    m_FocusBlock = m_NextBlock; // set the focus block to the next block
    m_FocusBlock.SetupSquares(BLOCK_START_X * SQUARE_SIZE,
                              BLOCK_START_Y * SQUARE_SIZE);

    // Set the next block to a new block of random type //
    m_OldNextBlock = m_NextBlock;
    m_NextBlock = cBlock(NEXT_BLOCK_CIRCLE_X * SQUARE_SIZE,
                         NEXT_BLOCK_CIRCLE_Y * SQUARE_SIZE, (rand() % 7 + 1));
}

// Return amount of lines cleared or zero if no lines were cleared //
int FallingBlocksGame::CheckCompletedLines()
{
    return m_OldSquares.CheckCompletedLines();
}

// Check to see if player has won. Handle winning condition if needed. //
void FallingBlocksGame::CheckWin()
{
    // If current level is greater than number of levels, player has won //
    if (m_Level > NUM_LEVELS)
    {
        // Clear the old squares container.
        m_OldSquares.Clear();

        // Reset score and level //
        m_Score = 0;
        m_Level = 1;

        // Pop all states //
        while (!m_StateStack.empty())
        {
            m_StateStack.pop();
        }

        // Push the victory state onto the stack //
//        m_StateStack.push(GAME_STATE_WON);
    }
}

// Check to see if player has lost. Handle losing condition if needed. //
void FallingBlocksGame::CheckLoss()
{
    // We call this function when the focus block is at the top of that //
    // game area. If the focus block is stuck now, the game is over.    //
    if ( CheckEntityCollisions(m_FocusBlock, DOWN) )
    {
        // Clear the old squares container.
        m_OldSquares.Clear();

        // Reset score and level //
        m_Score = 0;
        m_Level = 1;

        // Pop all states //
        while (!m_StateStack.empty())
        {
            m_StateStack.pop();
        }

        // Push the losing state onto the stack //
//        m_StateStack.push(GAME_STATE_LOST);
    }
}

//  Aaron Cox, 2004 //
//  Simon Que, 2013 //
