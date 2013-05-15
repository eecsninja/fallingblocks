//////////////////////////////////////////////////////////////////////////////////
// Screen.h
// - Handles drawing functions.
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdio.h>

#ifndef __AVR__
#include <SDL/SDL.h>
#else
struct SDL_Surface;
#endif

#include "Defines.h"
#include "Enums.h"

class Screen {
  private:
    SDL_Surface*   m_Bitmap;           // Our back bitmap
    SDL_Surface*   m_SquaresBitmap;    // Our squares bitmap
    SDL_Surface*   m_Window;           // Our backbuffer

#ifndef __AVR__
    // Rotating part of original palette.
    SDL_Color base_colors[255];
#endif

    int m_CurrentLevel;                // Current level, used for level colors.

  public:
    Screen() : m_Bitmap(NULL), m_SquaresBitmap(NULL), m_Window(NULL),
               m_CurrentLevel(0){}

    // Sets up and breaks down the video screen.
    void Init();
    void Cleanup();

    // Update video buffer.
    void Update();

    // Clear the video screen to black.
    void Clear();

    // Draw background.
    void DrawBackground(int level);

    // Draw a square at (x, y) of size (w, h) and |type|.
    void DrawSquare(int x, int y, int w, int h, int type);

    // Renders a string on the screen.
    void DisplayText(const char* text, int x, int y, int size,
                     int fR, int fG, int fB, int bR, int bG, int bB);
};

//  Aaron Cox, 2004 //
//  Simon Que, 2013 //
