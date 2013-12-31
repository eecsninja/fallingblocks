//////////////////////////////////////////////////////////////////////////////////
// Screen.cpp
// - Implements functions for class Screen.
//////////////////////////////////////////////////////////////////////////////////

#include "Screen.h"

#include <Arduino.h>
#include <DuinoCube.h>

#include "cSquare.h"
#include "Defines.h"

#define TILEMAP_WIDTH       32
#define TILEMAP_HEIGHT      32

namespace {

const char kFilePath[] = "falling";    // Base path of data files.

struct File {
  const char* filename;
  uint16_t* vram_offset;  // For image data, compute and store VRAM offset here.
  uint16_t addr;          // For other data, store data here.
  uint16_t bank;          // Bank to use for |addr|.
  uint16_t max_size;      // Size checking to avoid overflow.
};

// VRAM offsets of image data.
uint16_t g_font_offset;
uint16_t g_squares_offset;
uint16_t g_grass_offset;
uint16_t g_bricks_offset;

// Image, palette, and tilemap data.
const File kFiles[] = {
  // Layer data.
  { "bg.lay", NULL, TILEMAP(BG_LAYER_INDEX), TILEMAP_BANK, TILEMAP_SIZE },
  { "ui.lay", NULL, TILEMAP(UI_LAYER_INDEX), TILEMAP_BANK, TILEMAP_SIZE },

  // Palette data.
  { "font.pal", NULL, PALETTE(TEXT_PALETTE_INDEX), 0, PALETTE_SIZE },
  { "squares.pal", NULL, PALETTE(BLOCKS_PALETTE_INDEX), 0, PALETTE_SIZE },
  { "bricks.pal", NULL, PALETTE(UI_PALETTE_INDEX), 0, PALETTE_SIZE },
  { "grass.pal", NULL, PALETTE(BG_PALETTE_INDEX), 0, PALETTE_SIZE },

  // Image data.
  { "font.raw", &g_font_offset, 0, 0, VRAM_BANK_SIZE },
  { "squares.raw", &g_squares_offset, 0, 0, VRAM_BANK_SIZE },
  { "bricks.raw", &g_bricks_offset, 0, 0, VRAM_BANK_SIZE },
  { "grass.raw", &g_grass_offset, 0, 0, VRAM_BANK_SIZE },
};

// Different colors for the UI.
const Screen::Color kColors[] = {
    { 255,  84,  17 },
    { 202, 176,   2 },
    {   1,  99, 231 },
    { 255, 128,   0 },
    { 128,   0,  64 },
};

// Font colors.
#define BLACK    0x00000000
#define WHITE    0xffffffff

// Load image, palette, and tilemap data from file system.
void loadResources() {
  uint16_t vram_offset = 0;
  for (int i = 0; i < sizeof(kFiles) / sizeof(kFiles[0]); ++i) {
    const File& file = kFiles[i];

    char filename[256];
    sprintf(filename, "%s/%s", kFilePath, file.filename);

    // Open the file.
    uint16_t handle = DC.File.open(filename, FILE_READ_ONLY);
    if (!handle) {
      printf("Could not open file %s.\n", filename);
      continue;
    }

    uint16_t file_size = DC.File.size(handle);
    printf("File %s is 0x%x bytes\n", filename, file_size);

    if (file_size > file.max_size) {
      printf("File is too big!\n");
      DC.File.close(handle);
      continue;
    }

    // Compute write destination.
    uint16_t dest_addr;
    uint16_t dest_bank;

    if (file.vram_offset) {
      // Set up for VRAM write.

      // If this doesn't fit in the remaining part of the current bank, use the
      // next VRAM bank.
      if (vram_offset % VRAM_BANK_SIZE + file_size > VRAM_BANK_SIZE)
        vram_offset += VRAM_BANK_SIZE - (vram_offset % VRAM_BANK_SIZE);

      // Record VRAM offset.
      *file.vram_offset = vram_offset;

      // Determine the destination VRAM address and bank.
      dest_addr = VRAM_BASE + vram_offset % VRAM_BANK_SIZE;
      dest_bank = vram_offset / VRAM_BANK_SIZE + VRAM_BANK_BEGIN;
      DC.Core.writeWord(REG_SYS_CTRL, (1 << REG_SYS_CTRL_VRAM_ACCESS));

      // Update the VRAM offset.
      vram_offset += file_size;
    } else {
      // Set up for non-VRAM write.
      dest_addr = file.addr;
      dest_bank = file.bank;
    }

    printf("Writing to 0x%x with bank = %d\n", dest_addr, dest_bank);
    DC.Core.writeWord(REG_MEM_BANK, dest_bank);
    DC.File.readToCore(handle, dest_addr, file_size);

    DC.File.close(handle);
  }

  // Set to bank 0.
  DC.Core.writeWord(REG_MEM_BANK, 0);

  // Allow the graphics pipeline access to VRAM.
  DC.Core.writeWord(REG_SYS_CTRL, (0 << REG_SYS_CTRL_VRAM_ACCESS));
}

}  // namespace

void Screen::Init() {
    // Load game data.
    loadResources();

    // Copy VRAM offsets to member variables.
    m_FontDataOffset = g_font_offset;
    m_BGDataOffset = g_grass_offset;
    m_UIDataOffset = g_bricks_offset;
    m_BlocksDataOffset = g_squares_offset;

    // By default, map memory bank to the tilemap bank.  There's no need to
    // update VRAM during the game.
    DC.Core.writeWord(REG_MEM_BANK, TILEMAP_BANK);

    // Manually clear the text layer.
    uint16_t cleared_buffer[TILEMAP_WIDTH];
    memset(cleared_buffer, 0, sizeof(cleared_buffer));
    uint16_t offset = 0;
    for (int y = 0; y < TILEMAP_HEIGHT; ++y) {
      DC.Core.writeData(TILEMAP(TEXT_LAYER_INDEX) + offset,
                        cleared_buffer,
                        sizeof(cleared_buffer));
      offset += TILEMAP_WIDTH * BLOCK_TILE_ENTRY_SIZE;
    }

    // Set up two-color palette for font.
    DC.Core.writeWord(PALETTE_ENTRY(TEXT_PALETTE_INDEX, FONT_BLACK), BLACK);
    DC.Core.writeWord(PALETTE_ENTRY(TEXT_PALETTE_INDEX, FONT_WHITE), WHITE);

    // Set up and enable tile layers.
    DC.Core.writeWord(TILE_LAYER_REG(BG_LAYER_INDEX, TILE_CTRL_0),
                      (1 << TILE_LAYER_ENABLED) |
                      (BG_PALETTE_INDEX << TILE_PALETTE_START));
    DC.Core.writeWord(TILE_LAYER_REG(BG_LAYER_INDEX, TILE_DATA_OFFSET),
                      m_BGDataOffset);

    DC.Core.writeWord(TILE_LAYER_REG(UI_LAYER_INDEX, TILE_CTRL_0),
                      (1 << TILE_LAYER_ENABLED) |
                      (1 << TILE_ENABLE_NOP) |
                      (UI_PALETTE_INDEX << TILE_PALETTE_START));
    DC.Core.writeWord(TILE_LAYER_REG(UI_LAYER_INDEX, TILE_DATA_OFFSET),
                      m_UIDataOffset);
    DC.Core.writeWord(TILE_LAYER_REG(UI_LAYER_INDEX, TILE_EMPTY_VALUE),
                      DEFAULT_EMPTY_TILE_VALUE);

    DC.Core.writeWord(TILE_LAYER_REG(TEXT_LAYER_INDEX, TILE_CTRL_0),
                      (1 << TILE_LAYER_ENABLED) |
                      (1 << TILE_ENABLE_8x8) |
                      (1 << TILE_ENABLE_8_BIT) |
                      (1 << TILE_ENABLE_TRANSP) |
                      (TEXT_PALETTE_INDEX << TILE_PALETTE_START));
    DC.Core.writeWord(TILE_LAYER_REG(TEXT_LAYER_INDEX, TILE_DATA_OFFSET),
                      m_FontDataOffset);
    DC.Core.writeWord(TILE_LAYER_REG(TEXT_LAYER_INDEX, TILE_COLOR_KEY),
                      DEFAULT_TILE_COLOR_KEY);

    DC.Core.writeWord(TILE_LAYER_REG(BLOCKS_LAYER_INDEX, TILE_CTRL_0),
                      (1 << TILE_LAYER_ENABLED) |
                      (1 << TILE_ENABLE_NOP) |
                      (BLOCKS_PALETTE_INDEX << TILE_PALETTE_START));
    DC.Core.writeWord(TILE_LAYER_REG(BLOCKS_LAYER_INDEX, TILE_DATA_OFFSET),
                      m_BlocksDataOffset);
    DC.Core.writeWord(TILE_LAYER_REG(BLOCKS_LAYER_INDEX, TILE_EMPTY_VALUE),
                      DEFAULT_EMPTY_TILE_VALUE);

    // Set up UI color
    DrawBackground(1);
}

void Screen::Cleanup() {
}

void Screen::Update() {
}

void Screen::Clear() {
}

// This function draws the background //
void Screen::DrawBackground(int level) {
    if (m_CurrentLevel != level) {
        m_CurrentLevel = level;

        // Update the tileset instead.
        // Each tileset type has two tiles, so advance by two tiles
        uint16_t offset = m_UIDataOffset +
                         (level + 1) * SQUARE_SIZE * SQUARE_SIZE * 2;
        DC.Core.writeWord(TILE_LAYER_REG(UI_LAYER_INDEX, TILE_DATA_OFFSET),
                          offset);
    }

    // Clear the blocks layer.
    uint16_t buffer[GAME_AREA_WIDTH];
    for (int x = 0; x < sizeof(buffer) / sizeof(buffer[0]); ++x)
        buffer[x] = NO_BLOCK;

    // Start at the top left corner.
    uint16_t offset = GAME_AREA_LEFT * BLOCK_TILE_ENTRY_SIZE +
                      GAME_AREA_TOP* TILEMAP_WIDTH * BLOCK_TILE_ENTRY_SIZE;
    for (int y = GAME_AREA_TOP; y < GAME_AREA_BOTTOM; ++y) {
        DC.Core.writeData(TILEMAP(BLOCKS_LAYER_INDEX) + offset,
                          buffer, sizeof(buffer));
        offset += TILEMAP_WIDTH * BLOCK_TILE_ENTRY_SIZE;
    }
}

void Screen::DrawSquare(const cSquare& square) {
    int tile_x = square.GetX() / SQUARE_SIZE;
    int tile_y = square.GetY() / SQUARE_SIZE;
    uint16_t offset = (tile_x + tile_y * TILEMAP_WIDTH) * sizeof(uint16_t);
    DC.Core.writeWord(TILEMAP(BLOCKS_LAYER_INDEX) + offset, square.GetType());
}

void Screen::EraseSquare(const cSquare& square) {
    int tile_x = square.GetX() / SQUARE_SIZE;
    int tile_y = square.GetY() / SQUARE_SIZE;
    uint16_t offset = (tile_x + tile_y * TILEMAP_WIDTH) * sizeof(uint16_t);
    DC.Core.writeWord(TILEMAP(BLOCKS_LAYER_INDEX) + offset, NO_BLOCK);
}

void Screen::DisplayText(const char* text, int x, int y, int size,
                         int fR, int fG, int fB, int bR, int bG, int bB) {
    DC.Core.writeData(TILEMAP(TEXT_LAYER_INDEX) + x + y * TILEMAP_WIDTH * 2,
                      text, strlen(text));
}

void Screen::WaitForVblank() {
    while (!(DC.Core.readWord(REG_OUTPUT_STATUS) & (1 << REG_VBLANK)));
}

void Screen::WaitForNoVblank() {
    while ((DC.Core.readWord(REG_OUTPUT_STATUS) & (1 << REG_VBLANK)));
}

//  Aaron Cox, 2004 //
//  Simon Que, 2013 //
