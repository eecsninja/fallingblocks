//////////////////////////////////////////////////////////////////////////////////
// Project: Falling Blocks (Tetris)
// File:    Game.h
//////////////////////////////////////////////////////////////////////////////////

#ifndef __GAME_H__
#define __GAME_H__

#include <stdint.h>

#include "cBlock.h"  // Contains the class that represents a game block

#include "StateStack.h"   // Replaces stack<StatePointer>.
#include "LandedSquares.h"   // Replaces vector<cSquare>.
#include "Screen.h"          // Replaces SDL video functions.

// Game object containing all (previously) global game data.
class FallingBlocksGame {
  private:
    StateStack     m_StateStack;       // Our state stack
    Screen         m_Screen;           // Video screen controller.
    uint32_t       m_Timer;            // Our timer is just an integer
    cBlock         m_FocusBlock;       // The block the player is controlling
    cBlock         m_NextBlock;        // The next block to be the focus block
    cBlock         m_OldFocusBlock;    // The previous focus block.
    cBlock         m_OldNextBlock;     // The previous next block.
    LandedSquares  m_OldSquares;       // The squares that have landed.
    uint32_t       m_Score;            // Players current score
    int            m_Level;            // Current level player is on
    int            m_FocusBlockSpeed;  // Speed of the focus block

    // Used to avoid repeating pressing the up key.
    bool m_up_pressed;

    // These variables allow the user to hold the arrow keys down //
    bool m_down_pressed;
    bool m_left_pressed;
    bool m_right_pressed;

 public:
    FallingBlocksGame() : m_Score(0),
                          m_Level(1),
                          m_FocusBlockSpeed(INITIAL_SPEED),
                          m_up_pressed(false),
                          m_down_pressed(false),
                          m_left_pressed(false),
                          m_right_pressed(false)
                          {}

    // Init, Main Loop, and Shutdown functions //
    void Init();
    void MainLoop();
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
    void DisplayText(const char* text, int x, int y, int size,
                     int fR, int fG, int fB, int bR, int bG, int bB);
    void HandleMenuInput();
    void HandleGameInput();
    void HandleExitInput();
    void HandleWinLoseInput();
    bool CheckEntityCollisions(const cSquare& square, Direction dir);
    bool CheckWallCollisions(const cSquare& square, Direction dir);
    bool CheckEntityCollisions(const cBlock& block, Direction dir);
    bool CheckWallCollisions(const cBlock& block, Direction dir);
    bool CheckRotationCollisions(const cBlock& block);
    void CheckWin();
    void CheckLoss();
    void HandleBottomCollision();
    void ChangeFocusBlock();
    int CheckCompletedLines();
};

#endif  // __GAME_H__

//  Aaron Cox, 2004 //
//  Simon Que, 2013 //
