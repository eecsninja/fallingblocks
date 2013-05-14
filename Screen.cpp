//////////////////////////////////////////////////////////////////////////////////
// Screen.cpp
// - Implements functions for class Screen.
//////////////////////////////////////////////////////////////////////////////////

//#pragma comment(lib, "SDL_TTF.lib")

#include "Screen.h"

#include <SDL/SDL.h>

#ifdef RENDER_FONT
#include <SDL/SDL_ttf.h> // True Type Font header
#endif

#include "Defines.h"

namespace {
const char kBitmapFile[] = "data/FallingBlocks.bmp";
const char kSquaresFile[] = "data/squares.bmp";
}

void Screen::Init() {
    // Setup our window's dimensions, bits-per-pixel (0 tells SDL to choose for us), //
    // and video format (SDL_ANYFORMAT leaves the decision to SDL). This function    //
    // returns a pointer to our window which we assign to m_Window.                  //
    m_Window = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 0, SDL_ANYFORMAT);
    // Set the title of our window //
    SDL_WM_SetCaption(WINDOW_CAPTION, 0);

    // Fill our bitmap structure with information //
    m_Bitmap = SDL_LoadBMP(kBitmapFile);
    if (!m_Bitmap)
        fprintf(stderr, "Unable to open %s\n", kBitmapFile);
    m_SquaresBitmap = SDL_LoadBMP(kSquaresFile);
    if (!m_Bitmap)
        fprintf(stderr, "Unable to open %s\n", kSquaresFile);

    // Initialize the true type font library //
#ifdef RENDER_FONT
    TTF_Init();
#endif
}

void Screen::Cleanup() {
    // Free our surfaces //
    SDL_FreeSurface(m_Bitmap);
    SDL_FreeSurface(m_SquaresBitmap);
    SDL_FreeSurface(m_Window);

    // Shutdown the true type font library //
#ifdef RENDER_FONT
    TTF_Quit();
#endif
}

void Screen::Update() {
    // Tell SDL to display our backbuffer. The four 0's will make //
    // SDL display the whole screen. //
    SDL_UpdateRect(m_Window, 0, 0, 0, 0);
}

void Screen::Clear() {
    // This function just fills a surface with a given color. The //
    // first 0 tells SDL to fill the whole surface. The second 0  //
    // is for black. //
    SDL_FillRect(m_Window, 0, 0);
}

// This function draws the background //
void Screen::DrawBackground(int level)
{
    SDL_Rect source;

    // Set our source rectangle to the current level's background //
    switch (level)
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

    SDL_BlitSurface(m_Bitmap, &source, m_Window, &destination);
}

void Screen::DrawSquare(int x, int y, int w, int h, BlockType type) {

    // The bitmap of each color of square is arranged in the same order as the
    // block type enums.
    SDL_Rect source;
    source.w = SQUARE_SIZE;
    source.h = SQUARE_SIZE;
    source.x = 0;
    source.y = type * source.h;

    SDL_Rect destination = { x, y, w, h };
    SDL_BlitSurface(m_SquaresBitmap, &source, m_Window, &destination);
}

void Screen::DisplayText(const char* text, int x, int y, int size,
                         int fR, int fG, int fB, int bR, int bG, int bB)
{
#ifdef RENDER_FONT
    // Open our font and set its size to the given parameter //
    TTF_Font* font = TTF_OpenFont("arial.ttf", size);

    SDL_Color foreground  = { fR, fG, fB};   // text color
    SDL_Color background  = { bR, bG, bB };  // color of what's behind the text

    // This renders our text to a temporary surface. There //
    // are other text functions, but this one looks nicer. //
    SDL_Surface* temp = TTF_RenderText_Shaded(font, text, foreground, background);

    // A structure storing the destination of our text. //
    SDL_Rect destination = { x, y, 0, 0 };

    // Blit the text surface to our window surface. //
    SDL_BlitSurface(temp, NULL, m_Window, &destination);

    // Always free memory! //
    SDL_FreeSurface(temp);

    // Close the font. //
    TTF_CloseFont(font);
#endif
}

//  Aaron Cox, 2004 //
//  Simon Que, 2013 //
