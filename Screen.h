//////////////////////////////////////////////////////////////////////////////////
// Screen.h
// - Handles drawing functions.
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "Defines.h"
#include "Enums.h"

class cSquare;

class Screen {
  public:
    // Define a custom color struct.
    struct Color {
        uint8_t r, g, b;
        uint8_t unused;
    };

  private:
    // VRAM offsets for font, UI, and block images.
    uint16_t m_FontDataOffset;
    uint16_t m_BGDataOffset;
    uint16_t m_UIDataOffset;
    uint16_t m_BlocksDataOffset;

    // Rotating part of original palette.
    Color base_colors[NUM_CYCLED_COLORS];

    int m_CurrentLevel;                // Current level, used for level colors.

  public:
    Screen() : m_FontDataOffset(0),
               m_BGDataOffset(0),
               m_UIDataOffset(0),
               m_BlocksDataOffset(0),
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

    // Draw a square.
    void DrawSquare(const cSquare& square);

    // Erase a square.
    void EraseSquare(const cSquare& square);

    // Renders a string on the screen.
    void DisplayText(const char* text, int x, int y, int size,
                     int fR, int fG, int fB, int bR, int bG, int bB);

    // Wait for the vertical blanking to start and end.  If waiting for it to
    // start and it has already started, the function will immediately return.
    // Same for waiting for it to end and it already ended.
    void WaitForVblank();
    void WaitForNoVblank();
};

//  Aaron Cox, 2004 //
//  Simon Que, 2013 //
