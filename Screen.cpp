//////////////////////////////////////////////////////////////////////////////////
// Screen.cpp
// - Implements functions for class Screen.
//////////////////////////////////////////////////////////////////////////////////

#include "Screen.h"

#ifdef __AVR__

#include "cc_core.h"
#include "cc_tile_layer.h"
#include "registers.h"
#include "tile_registers.h"
#include "font_8x8.h"

// Image, palette, and tilemap data.
#include "Data/squares_palette.h"
#include "Data/squares_tileset.h"
#include "Data/ui_palette.h"
#include "Data/ui_tilemap.h"
#include "Data/ui_tileset.h"

#else

#include <SDL/SDL_ttf.h> // True Type Font header

#endif

#include "cSquare.h"
#include "Defines.h"

#define TILEMAP_WIDTH       32

namespace {

const char kBitmapFile[] = "data/ui.bmp";
const char kSquaresFile[] = "data/squares.bmp";

// Different colors for the UI.
const Screen::Color kColors[] = {
    { 255,  84,  17 },
    { 202, 176,   2 },
    {   1,  99, 231 },
    { 255, 128,   0 },
    { 128,   0,  64 },
};

}

void Screen::Init() {
    // Setup our window's dimensions, bits-per-pixel (0 tells SDL to choose for us), //
    // and video format (SDL_ANYFORMAT leaves the decision to SDL). This function    //
    // returns a pointer to our window which we assign to m_Window.                  //
#ifdef __AVR__
    CC_Init();

    // Load images.

    // Set up font.
    m_FontDataOffset = 0;
    uint16_t offset = m_FontDataOffset;
    for (int i = 0; i < NUM_FONT_CHARS; ++i) {
        char c = i;
        for (int line = 0; line < NUM_FONT_CHAR_LINES; ++line) {
            uint8_t line_data = get_font_line(c, line);
            uint8_t line_buffer[8];
            for (int b = 0; b < sizeof(line_buffer); ++b)
                line_buffer[b] = (line_data & (1 << b)) ? 1 : 0;
            CC_SetVRAMData(line_buffer, offset, sizeof(line_buffer));
            offset += sizeof(line_buffer);
        }
    }

    // Load UI image.
    m_UIDataOffset = offset;
    uint32_t value;
    for (int i = 0; i < UI_TILESET_BMP_RAW_DATA_SIZE / sizeof(value); ++i) {
        value = pgm_read_dword(&ui_tileset_bmp_raw_data32[i]);
        CC_SetVRAMData(&value, offset, sizeof(value));
        offset += sizeof(value);
    }

    // Load squares data.
    m_BlocksDataOffset = offset;
    for (int i = 0; i < SQUARES_BMP_RAW_DATA_SIZE / sizeof(value); ++i) {
        value = pgm_read_dword(&squares_bmp_raw_data32[i]);
        CC_SetVRAMData(&value, offset, sizeof(value));
        offset += sizeof(value);
    }

    // Load palette data.
    for (int i = 0; i < UI_TILESET_BMP_PAL_DATA_SIZE / sizeof(value); ++i) {
        value = pgm_read_dword(&ui_tileset_bmp_pal_data32[i]);
        CC_SetPaletteData(&value, UI_PALETTE_INDEX, i * sizeof(value),
                          sizeof(value));
    }
    for (int i = 0; i < NUM_CYCLED_COLORS; ++i) {
        value = pgm_read_dword(
                &ui_tileset_bmp_pal_data32[i + COLOR_CYCLING_START_INDEX]);
        memcpy(&base_colors[i], &value, sizeof(value));
    }

    for (int i = 0; i < SQUARES_BMP_PAL_DATA_SIZE / sizeof(value); ++i) {
        value = pgm_read_dword(&squares_bmp_pal_data32[i]);
        CC_SetPaletteData(&value, BLOCKS_PALETTE_INDEX, i * sizeof(value),
                          sizeof(value));
    }
    uint32_t black = 0x00000000;
    uint32_t white = 0xffffffff;
    CC_SetPaletteData(&black, TEXT_PALETTE_INDEX, 0, sizeof(black));
    CC_SetPaletteData(&white, TEXT_PALETTE_INDEX, sizeof(uint32_t) * 0xff,
                      sizeof(white));

    // Fill in UI tilemap.
    int x = 0, y = 0;
    for (int i = 0; i < UI_TMX_LAYER0_DAT_DATA_SIZE / sizeof(uint16_t); ++i) {
        uint16_t value = pgm_read_word(&ui_tmx_layer0_dat_data16[i]);
        CC_TileLayer_SetData(&value, UI_LAYER_INDEX,
                             (x + y * TILEMAP_WIDTH) * sizeof(value),
                             sizeof(value));
        ++x;
        // Tile map is not aligned to |TILEMAP_WIDTH|.  Its width is
        // |WINDOW_WIDTH| / |SQUARE_SIZE|.
        if (x >= WINDOW_WIDTH / SQUARE_SIZE) {
            x -= WINDOW_WIDTH / SQUARE_SIZE;
            ++y;
        }
    }

    // Set up and enable tile layers.
    CC_TileLayer_SetRegister(MENU_LAYER_INDEX, TILE_CTRL0,
                             (1 << TILE_LAYER_ENABLED) |
                             (1 << TILE_ENABLE_8x8) |
                             (1 << TILE_ENABLE_8_BIT) |
                             (TEXT_PALETTE_INDEX << TILE_PALETTE_START));
    CC_TileLayer_SetRegister(MENU_LAYER_INDEX, TILE_DATA_OFFSET,
                             m_FontDataOffset);

    CC_TileLayer_SetRegister(UI_LAYER_INDEX, TILE_CTRL0,
                             (1 << TILE_LAYER_ENABLED) |
                             (UI_PALETTE_INDEX << TILE_PALETTE_START));
    CC_TileLayer_SetRegister(UI_LAYER_INDEX, TILE_DATA_OFFSET, m_UIDataOffset);

    CC_TileLayer_SetRegister(TEXT_LAYER_INDEX, TILE_CTRL0,
                             (1 << TILE_LAYER_ENABLED) |
                             (1 << TILE_ENABLE_8x8) |
                             (1 << TILE_ENABLE_8_BIT) |
                             (1 << TILE_ENABLE_TRANSP) |
                             (TEXT_PALETTE_INDEX << TILE_PALETTE_START));
    CC_TileLayer_SetRegister(TEXT_LAYER_INDEX, TILE_DATA_OFFSET,
                             m_FontDataOffset);
    CC_TileLayer_SetRegister(TEXT_LAYER_INDEX, TILE_COLOR_KEY,
                             DEFAULT_TILE_COLOR_KEY);

    CC_TileLayer_SetRegister(BLOCKS_LAYER_INDEX, TILE_CTRL0,
                             (1 << TILE_LAYER_ENABLED) |
                             (1 << TILE_ENABLE_NOP) |
                             (BLOCKS_PALETTE_INDEX << TILE_PALETTE_START));
    CC_TileLayer_SetRegister(BLOCKS_LAYER_INDEX, TILE_DATA_OFFSET,
                             m_BlocksDataOffset);
    CC_TileLayer_SetRegister(BLOCKS_LAYER_INDEX, TILE_NOP_VALUE,
                             DEFAULT_EMPTY_TILE_VALUE);

    // Set up UI color
    DrawBackground(1);

#else
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
#endif  // defined(__AVR__)
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

        Color level_color;
        // Select a different UI color for each level.
        const int num_colors = sizeof(kColors) / sizeof(kColors[0]);
        if (level >= 0 && level < num_colors)
            level_color = kColors[level - 1];
        else
            level_color = kColors[num_colors - 1];

        // Update the palette with the new color scheme.
        for (int i = 0; i < NUM_CYCLED_COLORS; ++i) {
            Color new_color;
            new_color.r = ((uint16_t)base_colors[i].r * level_color.r) >> 8;
            new_color.g = ((uint16_t)base_colors[i].g * level_color.g) >> 8;
            new_color.b = ((uint16_t)base_colors[i].b * level_color.b) >> 8;
            new_color.unused = 0;
#ifdef __AVR__
            CC_SetPaletteData(
                    &new_color, UI_PALETTE_INDEX,
                    (i + COLOR_CYCLING_START_INDEX) * sizeof(new_color),
                    sizeof(new_color));
#else
            SDL_SetColors(m_Bitmap, &new_color, i + COLOR_CYCLING_START_INDEX,
                          1);
#endif  // defined(__AVR__)
        }
    }

#ifdef __AVR__
    // Clear the blocks layer.
    uint16_t buffer[GAME_AREA_WIDTH];
    for (int x = 0; x < GAME_AREA_WIDTH; ++x)
        buffer[x] = NO_BLOCK;
    for (int y = GAME_AREA_TOP; y < GAME_AREA_BOTTOM; ++y) {
        CC_TileLayer_SetData(
                buffer, BLOCKS_LAYER_INDEX,
                (y * TILEMAP_WIDTH + GAME_AREA_LEFT) * sizeof(uint16_t),
                sizeof(buffer));
    }
#else
    SDL_Rect destination = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    SDL_BlitSurface(m_Bitmap, NULL, m_Window, &destination);
#endif  // defined(__AVR__)
}

void Screen::DrawSquare(const cSquare& square) {
#ifdef __AVR__
    int tile_x = square.GetX() / SQUARE_SIZE;
    int tile_y = square.GetY() / SQUARE_SIZE;
    uint16_t tile_value = square.GetType();
    CC_TileLayer_SetData(&tile_value, BLOCKS_LAYER_INDEX,
                         (tile_x + tile_y * TILEMAP_WIDTH) * sizeof(tile_value),
                         sizeof(tile_value));
#else
    // The bitmap of each color of square is arranged in the same order as the
    // block type enums.
    SDL_Rect source;
    source.w = SQUARE_SIZE;
    source.h = SQUARE_SIZE;
    source.x = 0;
    source.y = type * source.h;

    SDL_Rect destination = { x, y, w, h };
    SDL_BlitSurface(m_SquaresBitmap, &source, m_Window, &destination);
#endif  // defined(__AVR__)
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

void Screen::WaitForVblank() {
#ifdef __AVR__
      while(!(CC_GetRegister(CC_REG_OUTPUT_STATUS) & (1 << CC_REG_VBLANK)));
#endif
}

void Screen::WaitForNoVblank() {
#ifdef __AVR__
      while(CC_GetRegister(CC_REG_OUTPUT_STATUS) & (1 << CC_REG_VBLANK));
#endif
}

//  Aaron Cox, 2004 //
//  Simon Que, 2013 //
