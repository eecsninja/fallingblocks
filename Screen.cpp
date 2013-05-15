//////////////////////////////////////////////////////////////////////////////////
// Screen.cpp
// - Implements functions for class Screen.
//////////////////////////////////////////////////////////////////////////////////

#include "Screen.h"

#ifndef __AVR__
#include <SDL/SDL_ttf.h> // True Type Font header
#endif

namespace {

const char kBitmapFile[] = "data/ui.bmp";
const char kSquaresFile[] = "data/squares.bmp";

#ifndef __AVR__
// Different colors for the UI.
const SDL_Color kColors[] = {
    { 255,  84,  17 },
    { 202, 176,   2 },
    {   1,  99, 231 },
    { 255, 128,   0 },
    { 128,   0,  64 },
};
#endif

}

void Screen::Init() {
    // Setup our window's dimensions, bits-per-pixel (0 tells SDL to choose for us), //
    // and video format (SDL_ANYFORMAT leaves the decision to SDL). This function    //
    // returns a pointer to our window which we assign to m_Window.                  //
#ifndef __AVR__
    m_Window = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 0, SDL_ANYFORMAT);
    // Set the title of our window //
    SDL_WM_SetCaption(WINDOW_CAPTION, 0);

    // Fill our bitmap structure with information //
    m_Bitmap = SDL_LoadBMP(kBitmapFile);
    if (!m_Bitmap)
        fprintf(stderr, "Unable to open %s\n", kBitmapFile);

    for (int i = 0; i < NUM_CYCLED_COLORS; ++i) {
        base_colors[i] =
            m_Bitmap->format->palette->colors[i + COLOR_CYCLING_START_INDEX];
    }

    m_SquaresBitmap = SDL_LoadBMP(kSquaresFile);
    if (!m_Bitmap)
        fprintf(stderr, "Unable to open %s\n", kSquaresFile);

    // Initialize the true type font library //
    TTF_Init();
#endif  // !defined(__AVR__)
}

void Screen::Cleanup() {
#ifndef __AVR__
    // Free our surfaces //
    SDL_FreeSurface(m_Bitmap);
    SDL_FreeSurface(m_SquaresBitmap);
    SDL_FreeSurface(m_Window);

    // Shutdown the true type font library //
    TTF_Quit();
#endif  // !defined(__AVR__)
}

void Screen::Update() {
#ifndef __AVR__
    // Tell SDL to display our backbuffer. The four 0's will make //
    // SDL display the whole screen. //
    SDL_UpdateRect(m_Window, 0, 0, 0, 0);
#endif
}

void Screen::Clear() {
#ifndef __AVR__
    // This function just fills a surface with a given color. The //
    // first 0 tells SDL to fill the whole surface. The second 0  //
    // is for black. //
    SDL_FillRect(m_Window, 0, 0);
#endif
}

// This function draws the background //
void Screen::DrawBackground(int level)
{
    if (m_CurrentLevel != level) {
        m_CurrentLevel = level;

#ifndef __AVR__
        SDL_Color level_color;
        // Select a different UI color for each level.
        const int num_colors = sizeof(kColors) / sizeof(kColors[0]);
        if (level >= 0 && level < num_colors)
            level_color = kColors[level - 1];
        else
            level_color = kColors[num_colors - 1];

        // Update the palette with the new color scheme.
        for (int i = 0; i < NUM_CYCLED_COLORS; ++i) {
            SDL_Color new_color;
            new_color.r = ((uint16_t)base_colors[i].r * level_color.r) >> 8;
            new_color.g = ((uint16_t)base_colors[i].g * level_color.g) >> 8;
            new_color.b = ((uint16_t)base_colors[i].b * level_color.b) >> 8;
            new_color.unused = 0;
            SDL_SetColors(m_Bitmap, &new_color, i + COLOR_CYCLING_START_INDEX,
                          1);
        }
#endif  // !defined(__AVR__)
    }

#ifndef __AVR__
    SDL_Rect destination = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    SDL_BlitSurface(m_Bitmap, NULL, m_Window, &destination);
#endif  // !defined(__AVR__)
}

void Screen::DrawSquare(int x, int y, int w, int h, int type) {
#ifndef __AVR__
    // The bitmap of each color of square is arranged in the same order as the
    // block type enums.
    SDL_Rect source;
    source.w = SQUARE_SIZE;
    source.h = SQUARE_SIZE;
    source.x = 0;
    source.y = type * source.h;

    SDL_Rect destination = { x, y, w, h };
    SDL_BlitSurface(m_SquaresBitmap, &source, m_Window, &destination);
#endif  // !defined(__AVR__)
}

void Screen::DisplayText(const char* text, int x, int y, int size,
                         int fR, int fG, int fB, int bR, int bG, int bB)
{
#ifndef __AVR__
    // Open our font and set its size to the given parameter //
    TTF_Font* font = TTF_OpenFont("arial.ttf", size);

    SDL_Color foreground  = { fR, fG, fB};   // text color
    SDL_Color background  = { bR, bG, bB };  // color of what's behind the text

    // This renders our text to a temporary surface. There //
    // are other text functions, but this one looks nicer. //
    SDL_Surface* temp = TTF_RenderText_Shaded(font, text, foreground, background);

    // A structure storing the destination of our text. //
    SDL_Rect destination = { x * SQUARE_SIZE, y * SQUARE_SIZE, 0, 0 };

    // Blit the text surface to our window surface. //
    SDL_BlitSurface(temp, NULL, m_Window, &destination);

    // Always free memory! //
    SDL_FreeSurface(temp);

    // Close the font. //
    TTF_CloseFont(font);
#endif  // !defined(__AVR__)
}

//  Aaron Cox, 2004 //
//  Simon Que, 2013 //
