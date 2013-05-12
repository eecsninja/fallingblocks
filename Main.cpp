//////////////////////////////////////////////////////////////////////////////////
// Project: Falling Blocks (Tetris)
// File:    Main.cpp
//////////////////////////////////////////////////////////////////////////////////

// These three lines link in the required SDL components for our project. //
// Alternatively, we could have linked them in our project settings.      //
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")
#pragma comment(lib, "SDL_TTF.lib")

#include <stack>     // We'll use the STL stack to store our function pointers
#include <vector>    // An STL vector will store the squares that are not part of the focus block
#include "time.h"    // We use time(), located in "time.h", to seed our random generator
#include "math.h"    // We'll be using the abs() function located in "math.h"
#include "SDL.h"     // Main SDL header
#include "SDL_TTF.h" // True Type Font header
#include "Defines.h" // Our defines header
#include "Enums.h"   // Our enums header
#include "cBlock.h"  // Contains the class that represents a game block

using namespace std;

// The STL stack can't take a function pointer as a type //
// so we encapsulate a function pointer within a struct. //
struct StateStruct
{
    void (*StatePointer)();
};

// Global data //
stack<StateStruct> g_StateStack;        // Our state stack
SDL_Surface*       g_Bitmap = NULL;     // Our back and squares bitmap
SDL_Surface*       g_Window = NULL;     // Our backbuffer
SDL_Event           g_Event;             // An SDL event structure for input
int                   g_Timer;             // Our timer is just an integer
cBlock*               g_FocusBlock = NULL; // The block the player is controlling
cBlock*               g_NextBlock  = NULL; // The next block to be the focus block
vector<cSquare*>   g_OldSquares;        // The squares that no longer form the focus block
int                   g_Score = 0;         // Players current score
int                   g_Level = 1;         // Current level player is on
int                   g_FocusBlockSpeed = INITIAL_SPEED; // Speed of the focus block

// Init and Shutdown functions //
void Init();
void Shutdown();

// Functions to handle the states of the game //
void Menu();
void Game();
void Exit();
void GameWon();
void GameLost();

// Helper functions for the main game state functions //
void DrawBackground();
void ClearScreen();
void DisplayText(string text, int x, int y, int size, int fR, int fG, int fB, int bR, int bG, int bB);
void HandleMenuInput();
void HandleGameInput();
void HandleExitInput();
void HandleWinLoseInput();
bool CheckEntityCollisions(cSquare* square, Direction dir);
bool CheckWallCollisions(cSquare* square, Direction dir);
bool CheckEntityCollisions(cBlock* block, Direction dir);
bool CheckWallCollisions(cBlock* block, Direction dir);
bool CheckRotationCollisions(cBlock* block);
void CheckWin();
void CheckLoss();
void HandleBottomCollision();
void ChangeFocusBlock();
int CheckCompletedLines();

int main(int argc, char **argv)
{
    Init();

    // Our game loop is just a while loop that breaks when our state stack is empty //
    while (!g_StateStack.empty())
    {
        g_StateStack.top().StatePointer();
    }

    Shutdown();

    return 0;
}


// This function initializes our game //
void Init()
{
    // Initiliaze SDL video and our timer //
    SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER);
    // Setup our window's dimensions, bits-per-pixel (0 tells SDL to choose for us), //
    // and video format (SDL_ANYFORMAT leaves the decision to SDL). This function    //
    // returns a pointer to our window which we assign to g_Window.                  //
    g_Window = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 0, SDL_ANYFORMAT);
    // Set the title of our window //
    SDL_WM_SetCaption(WINDOW_CAPTION, 0);
    // Get the number of ticks since SDL was initialized //
    g_Timer = SDL_GetTicks();

    // Fill our bitmap structure with information //
    g_Bitmap = SDL_LoadBMP("data/FallingBlocks.bmp");

    // Seed our random number generator //
    srand( time(0) );

    // Initialize blocks and set them to their proper locations. //
    g_FocusBlock = new cBlock(BLOCK_START_X, BLOCK_START_Y, g_Bitmap, (BlockType)(rand()%7));
    g_NextBlock  = new cBlock(NEXT_BLOCK_CIRCLE_X, NEXT_BLOCK_CIRCLE_Y, g_Bitmap, (BlockType)(rand()%7));

    // We start by adding a pointer to our exit state, this way //
    // it will be the last thing the player sees of the game.   //
    StateStruct state;
    state.StatePointer = Exit;
    g_StateStack.push(state);

    // Then we add a pointer to our menu state, this will //
    // be the first thing the player sees of our game.    //
    state.StatePointer = Menu;
    g_StateStack.push(state);

    // Initialize the true type font library //
    TTF_Init();
}

// This function shuts down our game //
void Shutdown()
{
    // Shutdown the true type font library //
    TTF_Quit();

    // Free our surfaces //
    SDL_FreeSurface(g_Bitmap);
    SDL_FreeSurface(g_Window);

    // Get pointers to the squares in our focus and next blocks so we can delete them. //
    // We must do this before we delete our blocks so we don't lose references to the squares. //
    // Note that these are pointers to arrays of pointers. //
    cSquare** temp_array_1 = g_FocusBlock->GetSquares();
    cSquare** temp_array_2 = g_NextBlock->GetSquares();

    // Delete our blocks //
    delete g_FocusBlock;
    delete g_NextBlock;

    // Delete the temporary arrays of squares //
    for (int i=0; i<4; i++)
    {
        delete temp_array_1[i];
        delete temp_array_2[i];
    }

    // Delete the squares that are in the game area //
    for (int i=0; i<g_OldSquares.size(); i++)
    {
        delete g_OldSquares[i];
    }

    // Tell SDL to shutdown and free any resources it was using //
    SDL_Quit();
}

// This function handles the game's main menu. From here //
// the player can select to enter the game, or quit.     //
void Menu()
{
    // Here we compare the difference between the current time and the last time we //
    // handled a frame. If FRAME_RATE amount of time has passed, it's time for a new frame. //
    if ( (SDL_GetTicks() - g_Timer) >= FRAME_RATE )
    {
        // We start by calling our input function //
        HandleMenuInput();

        // Make sure nothing from the last frame is still drawn //
        ClearScreen();

        DisplayText("Start (G)ame", 120, 120, 12, 255, 255, 255, 0, 0, 0);
        DisplayText("(Q)uit Game",  120, 150, 12, 255, 255, 255, 0, 0, 0);

        // Tell SDL to display our backbuffer. The four 0's will make //
        // SDL display the whole screen. //
        SDL_UpdateRect(g_Window, 0, 0, 0, 0);

        // We've processed a frame so we now need to record the time at which we did it. //
        // This way we can compare this time with the next time our function gets called //
        // and see if enough time has passed. //
        g_Timer = SDL_GetTicks();
    }
}

// This function handles the main game. We'll control the   //
// drawing of the game as well as any necessary game logic. //
void Game()
{
    // Every frame we increase this value until it is equal to g_FocusBlockSpeed. //
    // When it reaches that value, we force the focus block down. //
    static int force_down_counter = 0;

    // Every frame, we check to see if the focus block's bottom has hit something. If it    //
    // has, we decrement this counter. If the counter hits zero, the focus block needs to   //
    // be changed. We use this counter so the player can slide the block before it changes. //
    static int slide_counter = SLIDE_TIME;

    // Here we compare the difference between the current time and the last time we //
    // handled a frame. If FRAME_RATE amount of time has, it's time for a new frame. //
    if ( (SDL_GetTicks() - g_Timer) >= FRAME_RATE )
    {
        HandleGameInput();

        force_down_counter++;

        if (force_down_counter >= g_FocusBlockSpeed)
        {
            // Always check for collisions before moving anything //
            if ( !CheckWallCollisions(g_FocusBlock, DOWN) && !CheckEntityCollisions(g_FocusBlock, DOWN) )
            {
                g_FocusBlock->Move(DOWN); // move the focus block
                force_down_counter = 0;   // reset our counter
            }
        }

        // Check to see if focus block's bottom has hit something. If it has, we decrement our counter. //
        if ( CheckWallCollisions(g_FocusBlock, DOWN) || CheckEntityCollisions(g_FocusBlock, DOWN) )
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

        // Make sure nothing from the last frame is still drawn. //
        ClearScreen();

        // Draw the background //
        DrawBackground();

        // Draw the focus block and next block. //
        g_FocusBlock->Draw(g_Window);
        g_NextBlock->Draw(g_Window);

        // Draw the old squares. //
        for (int i=0; i < g_OldSquares.size(); i++)
        {
            g_OldSquares[i]->Draw(g_Window);
        }

        // Draw the text for the current level, score, and needed score. //

        // We need to display the text ("Score:", "Level:", "Needed Score:") as well as the //
        // associated value. To do this, we use the string function append(). This function //
        // takes a char string so we call itoa() and store the char string in temp.         //
        char temp[256];

        string score = "Score: ";
        itoa(g_Score, temp, 10);
        score.append( temp );

        string nextscore = "Needed Score: ";
        itoa(g_Level*POINTS_PER_LEVEL, temp, 10);
        nextscore.append(temp);

        string level = "Level: ";
        itoa(g_Level, temp, 10);
        level.append(temp);

        DisplayText(score, SCORE_RECT_X, SCORE_RECT_Y, 8, 0, 0, 0, 255, 255, 255);
        DisplayText(nextscore, NEEDED_SCORE_RECT_X, NEEDED_SCORE_RECT_Y, 8, 0, 0, 0, 255, 255, 255);
        DisplayText(level, LEVEL_RECT_X, LEVEL_RECT_Y, 8, 0, 0, 0, 255, 255, 255);

        // Tell SDL to display our backbuffer. The four 0's will make //
        // SDL display the whole screen. //
        SDL_UpdateRect(g_Window, 0, 0, 0, 0);

        // We've processed a frame so we now need to record the time at which we did it. //
        // This way we can compare this time the next time our function gets called and  //
        // see if enough time has passed between iterations. //
        g_Timer = SDL_GetTicks();
    }
}

// This function handles the game's exit screen. It will display //
// a message asking if the player really wants to quit.          //
void Exit()
{
    // Here we compare the difference between the current time and the last time we //
    // handled a frame. If FRAME_RATE amount of time has, it's time for a new frame. //
    if ( (SDL_GetTicks() - g_Timer) >= FRAME_RATE )
    {
        HandleExitInput();

        // Make sure nothing from the last frame is still drawn. //
        ClearScreen();

        DisplayText("Quit Game (Y or N)?", 100, 150, 12, 255, 255, 255, 0, 0, 0);

        // Tell SDL to display our backbuffer. The four 0's will make //
        // SDL display the whole screen. //
        SDL_UpdateRect(g_Window, 0, 0, 0, 0);

        // We've processed a frame so we now need to record the time at which we did it. //
        // This way we can compare this time the next time our function gets called and  //
        // see if enough time has passed between iterations. //
        g_Timer = SDL_GetTicks();
    }
}

// Display a victory message. //
void GameWon()
{
    if ( (SDL_GetTicks() - g_Timer) >= FRAME_RATE )
    {
        HandleWinLoseInput();

        ClearScreen();

        DisplayText("You Win!!!", 100, 120, 12, 255, 255, 255, 0, 0, 0);
        DisplayText("Quit Game (Y or N)?", 100, 140, 12, 255, 255, 255, 0, 0, 0);

        SDL_UpdateRect(g_Window, 0, 0, 0, 0);

        g_Timer = SDL_GetTicks();
    }
}

// Display a game over message. //
void GameLost()
{
    if ( (SDL_GetTicks() - g_Timer) >= FRAME_RATE )
    {
        HandleWinLoseInput();

        ClearScreen();

        DisplayText("You Lose.", 100, 120, 12, 255, 255, 255, 0, 0, 0);
        DisplayText("Quit Game (Y or N)?", 100, 140, 12, 255, 255, 255, 0, 0, 0);

        SDL_UpdateRect(g_Window, 0, 0, 0, 0);

        g_Timer = SDL_GetTicks();
    }
}

// This function draws the background //
void DrawBackground()
{
    SDL_Rect source;

    // Set our source rectangle to the current level's background //
    switch (g_Level)
    {
    case 1:
        {
        SDL_Rect temp = { LEVEL_ONE_X, LEVEL_ONE_Y, WINDOW_WIDTH, WINDOW_HEIGHT };
        source = temp;
        } break;
    case 2:
        {
        SDL_Rect temp = { LEVEL_TWO_X, LEVEL_TWO_Y, WINDOW_WIDTH, WINDOW_HEIGHT };
        source = temp;
        } break;
    case 3:
        {
        SDL_Rect temp = { LEVEL_THREE_X, LEVEL_THREE_Y, WINDOW_WIDTH, WINDOW_HEIGHT };
        source = temp;
        } break;
    case 4:
        {
        SDL_Rect temp = { LEVEL_FOUR_X, LEVEL_FOUR_Y, WINDOW_WIDTH, WINDOW_HEIGHT };
        source = temp;
        } break;
    case 5:
        {
        SDL_Rect temp = { LEVEL_FIVE_X, LEVEL_FIVE_Y, WINDOW_WIDTH, WINDOW_HEIGHT };
        source = temp;
        } break;
    }

    SDL_Rect destination = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

    SDL_BlitSurface(g_Bitmap, &source, g_Window, &destination);
}

// This function simply clears the back buffer to black //
void ClearScreen()
{
    // This function just fills a surface with a given color. The //
    // first 0 tells SDL to fill the whole surface. The second 0  //
    // is for black. //
    SDL_FillRect(g_Window, 0, 0);
}

// This function displays text to the screen. It takes the text //
// to be displayed, the location to display it, the size of the //
// text, and the color of the text and background.              //
void DisplayText(string text, int x, int y, int size, int fR, int fG, int fB, int bR, int bG, int bB)
{
    // Open our font and set its size to the given parameter //
    TTF_Font* font = TTF_OpenFont("arial.ttf", size);

    SDL_Color foreground  = { fR, fG, fB};   // text color
    SDL_Color background  = { bR, bG, bB };  // color of what's behind the text

    // This renders our text to a temporary surface. There //
    // are other text functions, but this one looks nicer. //
    SDL_Surface* temp = TTF_RenderText_Shaded(font, text.c_str(), foreground, background);

    // A structure storing the destination of our text. //
    SDL_Rect destination = { x, y, 0, 0 };

    // Blit the text surface to our window surface. //
    SDL_BlitSurface(temp, NULL, g_Window, &destination);

    // Always free memory! //
    SDL_FreeSurface(temp);

    // Close the font. //
    TTF_CloseFont(font);
}

// This function receives player input and //
// handles it for the game's menu screen.  //
void HandleMenuInput()
{
    // Fill our event structure with event information. //
    if ( SDL_PollEvent(&g_Event) )
    {
        // Handle user manually closing game window //
        if (g_Event.type == SDL_QUIT)
        {
            // While state stack isn't empty, pop //
            while (!g_StateStack.empty())
            {
                g_StateStack.pop();
            }

            return;  // game is over, exit the function
        }

        // Handle keyboard input here //
        if (g_Event.type == SDL_KEYDOWN)
        {
            if (g_Event.key.keysym.sym == SDLK_ESCAPE)
            {
                g_StateStack.pop();
                return;  // this state is done, exit the function
            }
            // Quit //
            if (g_Event.key.keysym.sym == SDLK_q)
            {
                g_StateStack.pop();
                return;  // game is over, exit the function
            }
            // Start Game //
            if (g_Event.key.keysym.sym == SDLK_g)
            {
                StateStruct temp;
                temp.StatePointer = Game;
                g_StateStack.push(temp);
                return;  // this state is done, exit the function
            }
        }
    }
}

// This function receives player input and //
// handles it for the main game state.     //
void HandleGameInput()
{
    // These variables allow the user to hold the arrow keys down //
    static bool down_pressed  = false;
    static bool left_pressed  = false;
    static bool right_pressed = false;

    // Fill our event structure with event information. //
    if ( SDL_PollEvent(&g_Event) )
    {
        // Handle user manually closing game window //
        if (g_Event.type == SDL_QUIT)
        {
            // While state stack isn't empty, pop //
            while (!g_StateStack.empty())
            {
                g_StateStack.pop();
            }

            return;  // game is over, exit the function
        }

        // Handle keyboard input here //
        if (g_Event.type == SDL_KEYDOWN)
        {
            if (g_Event.key.keysym.sym == SDLK_ESCAPE)
            {
                g_StateStack.pop();

                return;  // this state is done, exit the function
            }

            if (g_Event.key.keysym.sym == SDLK_UP)
            {
                // Check collisions before rotating //
                if (!CheckRotationCollisions(g_FocusBlock))
                {
                    g_FocusBlock->Rotate();
                }
            }

            // For the left, right, and down arrow keys, we just set a bool variable //
            if (g_Event.key.keysym.sym == SDLK_LEFT)
            {
                left_pressed = true;
            }
            if (g_Event.key.keysym.sym == SDLK_RIGHT)
            {
                right_pressed = true;
            }
            if (g_Event.key.keysym.sym == SDLK_DOWN)
            {
                down_pressed = true;
            }
        }

        // If player lifts key, set bool variable to false //
        if (g_Event.type == SDL_KEYUP)
        {
            if (g_Event.key.keysym.sym == SDLK_LEFT)
            {
                left_pressed = false;
            }
            if (g_Event.key.keysym.sym == SDLK_RIGHT)
            {
                right_pressed = false;
            }
            if (g_Event.key.keysym.sym == SDLK_DOWN)
            {
                down_pressed = false;
            }
        }
    }

    // Now we handle the arrow keys, making sure to check for collisions //
    if (down_pressed)
    {
        if ( !CheckWallCollisions(g_FocusBlock, DOWN) &&
             !CheckEntityCollisions(g_FocusBlock, DOWN) )
        {
            g_FocusBlock->Move(DOWN);
        }
    }
    if (left_pressed)
    {
        if ( !CheckWallCollisions(g_FocusBlock, LEFT) &&
             !CheckEntityCollisions(g_FocusBlock, LEFT) )
        {
            g_FocusBlock->Move(LEFT);
        }
    }
    if (right_pressed)
    {
        if ( !CheckWallCollisions(g_FocusBlock, RIGHT) &&
             !CheckEntityCollisions(g_FocusBlock, RIGHT) )
        {
            g_FocusBlock->Move(RIGHT);
        }
    }
}

// This function receives player input and //
// handles it for the game's exit screen.  //
void HandleExitInput()
{
    // Fill our event structure with event information. //
    if ( SDL_PollEvent(&g_Event) )
    {
        // Handle user manually closing game window //
        if (g_Event.type == SDL_QUIT)
        {
            // While state stack isn't empty, pop //
            while (!g_StateStack.empty())
            {
                g_StateStack.pop();
            }

            return;  // game is over, exit the function
        }

        // Handle keyboard input here //
        if (g_Event.type == SDL_KEYDOWN)
        {
            if (g_Event.key.keysym.sym == SDLK_ESCAPE)
            {
                g_StateStack.pop();

                return;  // this state is done, exit the function
            }
            // Yes //
            if (g_Event.key.keysym.sym == SDLK_y)
            {
                g_StateStack.pop();
                return;  // game is over, exit the function
            }
            // No //
            if (g_Event.key.keysym.sym == SDLK_n)
            {
                StateStruct temp;
                temp.StatePointer = Menu;
                g_StateStack.push(temp);
                return;  // this state is done, exit the function
            }
        }
    }
}

// Input handling for win/lose screens. //
void HandleWinLoseInput()
{
    if ( SDL_PollEvent(&g_Event) )
    {
        // Handle user manually closing game window //
        if (g_Event.type == SDL_QUIT)
        {
            // While state stack isn't empty, pop //
            while (!g_StateStack.empty())
            {
                g_StateStack.pop();
            }

            return;
        }

        // Handle keyboard input here //
        if (g_Event.type == SDL_KEYDOWN)
        {
            if (g_Event.key.keysym.sym == SDLK_ESCAPE)
            {
                g_StateStack.pop();

                return;
            }
            if (g_Event.key.keysym.sym == SDLK_y)
            {
                g_StateStack.pop();
                return;
            }
            // If player chooses to continue playing, we pop off    //
            // current state and push exit and menu states back on. //
            if (g_Event.key.keysym.sym == SDLK_n)
            {
                g_StateStack.pop();

                StateStruct temp;
                temp.StatePointer = Exit;
                g_StateStack.push(temp);

                temp.StatePointer = Menu;
                g_StateStack.push(temp);
                return;
            }
        }
    }
}

// Check collisions between a given square and the squares in g_OldSquares //
bool CheckEntityCollisions(cSquare* square, Direction dir)
{
    // Width/height of a square. Also the distance //
    // between two squares if they've collided.    //
    int distance = SQUARE_MEDIAN * 2;

    // Center of the given square //
    int centerX = square->GetCenterX();
    int centerY = square->GetCenterY();

    // Determine the location of the square after moving //
    switch (dir)
    {
    case DOWN:
        {
            centerY += distance;
        } break;

    case LEFT:
        {
            centerX -= distance;
        } break;

    case RIGHT:
        {
            centerX += distance;
        } break;
    }

    // Iterate through the old squares vector, checking for collisions //
    for (int i=0; i<g_OldSquares.size(); i++)
    {
        if ( ( abs(centerX - g_OldSquares[i]->GetCenterX() ) < distance ) &&
             ( abs(centerY - g_OldSquares[i]->GetCenterY() ) < distance ) )
        {
            return true;
        }
    }

    return false;
}

// Check collisions between a given block and the squares in g_OldSquares //
bool CheckEntityCollisions(cBlock* block, Direction dir)
{
    // Get an array of the squares that make up the given block //
    cSquare** temp_array = block->GetSquares();

    // Now just call the other CheckEntityCollisions() on each square //
    for (int i=0; i<4; i++)
    {
        if ( CheckEntityCollisions(temp_array[i], dir) )
            return true;
    }

    return false;
}

// Check collisions between a given square and the sides of the game area //
bool CheckWallCollisions(cSquare* square, Direction dir)
{
    // Get the center of the square //
    int x = square->GetCenterX();
    int y = square->GetCenterY();

    // Get the location of the square after moving and see if its out of bounds //
    switch (dir)
    {
    case DOWN:
        {
            if ( (y + (SQUARE_MEDIAN*2)) > GAME_AREA_BOTTOM )
            {
                return true;
            }
            else
            {
                return false;
            }
        } break;

    case LEFT:
        {
            if ( (x - (SQUARE_MEDIAN*2)) < GAME_AREA_LEFT )
            {
                return true;
            }
            else
            {
                return false;
            }
        } break;

    case RIGHT:
        {
            if ( (x + (SQUARE_MEDIAN*2)) > GAME_AREA_RIGHT )
            {
                return true;
            }
            else
            {
                return false;
            }
        } break;
    }

    return false;
}

// Check for collisions between a given block a the sides of the game area //
bool CheckWallCollisions(cBlock* block, Direction dir)
{
    // Get an array of squares that make up the given block //
    cSquare** temp_array = block->GetSquares();

    // Call other CheckWallCollisions() on each square //
    for (int i=0; i<4; i++)
    {
        if ( CheckWallCollisions(temp_array[i], dir) )
            return true;
    }

    return false;
}

// Check for collisions when a block is rotated //
bool CheckRotationCollisions(cBlock* block)
{
    // Get an array of values for the locations of the rotated block's squares //
    int* temp_array = block->GetRotatedSquares();

    // Distance between two touching squares //
    int distance = SQUARE_MEDIAN * 2;

    for (int i=0; i<4; i++)
    {
        // Check to see if the block will go out of bounds //
        if ( (temp_array[i*2] < GAME_AREA_LEFT) || (temp_array[i*2] > GAME_AREA_RIGHT) )
        {
            delete temp_array;
            return true;
        }

        if ( temp_array[i*2+1] > GAME_AREA_BOTTOM )
        {
            delete temp_array;
            return true;
        }

        // Check to see if the block will collide with any squares //
        for (int index=0; index<g_OldSquares.size(); index++)
        {
            if ( ( abs(temp_array[i*2]   - g_OldSquares[index]->GetCenterX()) < distance ) &&
                 ( abs(temp_array[i*2+1] - g_OldSquares[index]->GetCenterY()) < distance ) )
            {
                delete temp_array;
                return true;
            }
        }
    }

    delete temp_array;
    return false;
}

// This function handles all of the events that   //
// occur when the focus block can no longer move. //
void HandleBottomCollision()
{
    ChangeFocusBlock();

    // Check for completed lines and store the number of lines completed //
    int num_lines = CheckCompletedLines();

    if ( num_lines > 0 )
    {
        // Increase player's score according to number of lines completed //
        g_Score += POINTS_PER_LINE * num_lines;

        // Check to see if it's time for a new level //
        if (g_Score >= g_Level * POINTS_PER_LEVEL)
        {
            g_Level++;
            CheckWin(); // check for a win after increasing the level
            g_FocusBlockSpeed -= SPEED_CHANGE; // shorten the focus blocks movement interval
        }
    }

    // Now would be a good time to check to see if the player has lost //
    CheckLoss();
}

// Add the squares of the focus block to g_OldSquares //
// and set the next block as the focus block. //
void ChangeFocusBlock()
{
    // Get an array of pointers to the focus block squares //
    cSquare** square_array = g_FocusBlock->GetSquares();

    // Add focus block squares to g_OldSquares //
    for (int i=0; i<4; i++)
    {
        g_OldSquares.push_back(square_array[i]);
    }

    delete g_FocusBlock;        // delete the current focus block
    g_FocusBlock = g_NextBlock; // set the focus block to the next block
    g_FocusBlock->SetupSquares(BLOCK_START_X, BLOCK_START_Y, g_Bitmap);

    // Set the next block to a new block of random type //
    g_NextBlock = new cBlock(NEXT_BLOCK_CIRCLE_X, NEXT_BLOCK_CIRCLE_Y, g_Bitmap, (BlockType)(rand()%7));
}

// Return amount of lines cleared or zero if no lines were cleared //
int CheckCompletedLines()
{
    // Store the amount of squares in each row in an array //
    int squares_per_row[13];

    // The compiler will fill the array with junk values if we don't do this //
    for (int index=0; index<13; index++)
        squares_per_row[index] = 0;

    int row_size   = SQUARE_MEDIAN * 2;                // pixel size of one row
    int bottom     = GAME_AREA_BOTTOM - SQUARE_MEDIAN; // center of bottom row
    int top        = bottom - 12 * row_size;           // center of top row

    int num_lines = 0; // number of lines cleared
    int row;           // multipurpose variable


    // Check for full lines //
    for (int i=0; i<g_OldSquares.size(); i++)
    {
        // Get the row the current square is in //
        row = (g_OldSquares[i]->GetCenterY() - top) / row_size;

        // Increment the appropriate row counter //
        squares_per_row[row]++;
    }

    // Erase any full lines //
    for (int line=0; line<13; line++)
    {
        // Check for completed lines //
        if (squares_per_row[line] == SQUARES_PER_ROW)
        {
            // Keep track of how many lines have been completed //
            num_lines++;

            // Find any squares in current row and remove them //
            for (int index=0; index<g_OldSquares.size(); index++)
            {
                if ( ( (g_OldSquares[index]->GetCenterY() - top) / row_size ) == line )
                {
                    delete g_OldSquares[index]; // delete the square
                    g_OldSquares.erase(g_OldSquares.begin() + index); // remove it from the vector
                    index--; // make sure we don't skip anything
                }
            }
        }
    }

    // Move squares above cleared line down //
    for (int index=0; index<g_OldSquares.size(); index++)
    {
        for (int line=0; line<13; line++)
        {
            // Determine if this row was filled //
            if (squares_per_row[line] == SQUARES_PER_ROW)
            {
                // If it was, get the location of it within the game area //
                row = (g_OldSquares[index]->GetCenterY() - top) / row_size;

                // Now move any squares above that row down one //
                if ( row < line )
                {
                    g_OldSquares[index]->Move(DOWN);
                }
            }
        }
    }

    return num_lines;
}

// Check to see if player has won. Handle winning condition if needed. //
void CheckWin()
{
    // If current level is greater than number of levels, player has won //
    if (g_Level > NUM_LEVELS)
    {
        // Clear the old squares vector //
        for (int i=0; i<g_OldSquares.size(); i++)
        {
            delete g_OldSquares[i];
        }
        g_OldSquares.clear();

        // Reset score and level //
        g_Score = 0;
        g_Level = 1;

        // Pop all states //
        while (!g_StateStack.empty())
        {
            g_StateStack.pop();
        }

        // Push the victory state onto the stack //
        StateStruct win;
        win.StatePointer = GameWon;

        g_StateStack.push(win);
    }
}

// Check to see if player has lost. Handle losing condition if needed. //
void CheckLoss()
{
    // We call this function when the focus block is at the top of that //
    // game area. If the focus block is stuck now, the game is over.    //
    if ( CheckEntityCollisions(g_FocusBlock, DOWN) )
    {
        // Clear the old squares vector //
        for (int i=0; i<g_OldSquares.size(); i++)
        {
            delete g_OldSquares[i];
        }
        g_OldSquares.clear();

        // Reset score and level //
        g_Score = 0;
        g_Level = 1;

        // Pop all states //
        while (!g_StateStack.empty())
        {
            g_StateStack.pop();
        }

        // Push the losing state onto the stack //
        StateStruct lose;
        lose.StatePointer = GameLost;

        g_StateStack.push(lose);
    }
}

//  Aaron Cox, 2004 //
