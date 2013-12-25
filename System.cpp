////////////////////////////////////////////////////////////////////////////////
// System.cpp
// - Implements timing and input functions.
////////////////////////////////////////////////////////////////////////////////

#include "System.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Arduino.h>
#include <DuinoCube.h>

namespace System {

    bool Init() {
        return true;
    }

    KeyState GetKeyState() {
        KeyState key_state;
        memset(&key_state, 0, sizeof(key_state));

        // Read and interpret gamepad state.
        GamepadState state = DC.Gamepad.readGamepad();

        if (state.buttons & (1 << GAMEPAD_BUTTON_1))
            key_state.quit = 1;
        if (state.buttons & (1 << GAMEPAD_BUTTON_4))
            key_state.pause = 1;
        if (state.buttons & (1 << GAMEPAD_BUTTON_2))
            key_state.new_game = 1;
        if (state.buttons & (1 << GAMEPAD_BUTTON_2))
            key_state.yes = 1;
        if (state.buttons & (1 << GAMEPAD_BUTTON_1))
            key_state.no = 1;
        if (state.y == 0)
            key_state.up = 1;
        if (state.y == UINT8_MAX)
            key_state.down = 1;
        if (state.x == 0)
            key_state.left = 1;
        if (state.x == UINT8_MAX)
            key_state.right = 1;

        return key_state;
    }

    uint32_t GetTicks() {
        return millis();
    }

}  // namespace System
