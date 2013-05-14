//////////////////////////////////////////////////////////////////////////////////
// Screen.h
// - Handles drawing functions.
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdio.h>

#include "Enums.h"

struct SDL_Surface;

class Screen {
  private:
    SDL_Surface*   m_Bitmap;           // Our back bitmap
    SDL_Surface*   m_SquaresBitmap;    // Our squares bitmap
    SDL_Surface*   m_Window;           // Our backbuffer

  public:
    Screen() : m_Bitmap(NULL), m_SquaresBitmap(NULL), m_Window(NULL) {}

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
