////////////////////////////////////////////////////////////////////////////////
// System.cpp
// - Implements timing and input functions.
////////////////////////////////////////////////////////////////////////////////

#include "System.h"

#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>

namespace System {

    bool Init() {
        atexit(SDL_Quit);
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
            fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
            return false;
        }
        return true;
    }

    KeyState GetKeyState() {
        KeyState key_state;
        memset(&key_state, 0, sizeof(key_state));

        // Get the current keys from SDL.  Call SDL_PollEvent() to update the
        // key array with the latest values.
        Uint8* keys = SDL_GetKeyState(NULL);
        SDL_Event event;
        while(SDL_PollEvent(&event));

        if (keys[SDLK_ESCAPE])
            key_state.quit = 1;
        if (keys[SDLK_p])
            key_state.pause = 1;
        if (keys[SDLK_g])
            key_state.new_game = 1;
        if (keys[SDLK_y])
            key_state.yes = 1;
        if (keys[SDLK_n])
            key_state.no = 1;
        if (keys[SDLK_UP])
            key_state.up = 1;
        if (keys[SDLK_DOWN])
            key_state.down = 1;
        if (keys[SDLK_LEFT])
            key_state.left = 1;
        if (keys[SDLK_RIGHT])
            key_state.right = 1;

        return key_state;
    }

    uint32_t GetTicks() {
        return SDL_GetTicks();
    }

}  // namespace System
