//////////////////////////////////////////////////////////////////////////////////
// Project: Falling Blocks (Tetris)
// File:    FallingBlocks.ino
//////////////////////////////////////////////////////////////////////////////////

#include <DuinoCube.h>
#include <SPI.h>

#include "Game.h"
#include "System.h"          // Replaces SDL timer and input functions.

void setup() {
  Serial.begin(115200);
  DC.begin();

  FallingBlocksGame game;
  printf("Game data size: %u bytes\n", sizeof(game));

  game.Init();

  game.MainLoop();
  game.Shutdown();
}

void loop() {
}

//  Simon Que, 2013 //
