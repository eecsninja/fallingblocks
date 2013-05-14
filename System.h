////////////////////////////////////////////////////////////////////////////////
// System.h
// - Handles timing and input.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdint.h>

namespace System {

    // Bitfield struct containing state of all the relevant keys.  Each key bit
    // is set if the key is pressed.
    struct KeyState {
        uint8_t pause     :1;
        uint8_t quit      :1;
        uint8_t new_game  :1;
        uint8_t yes       :1;
        uint8_t no        :1;
        uint8_t up        :1;
        uint8_t down      :1;
        uint8_t left      :1;
        uint8_t right     :1;
    };

    // Initializes system resources.
    bool Init();

    // Returns the current key state.
    KeyState GetKeyState();

    // Returns the number of ticks on a system timer.
    uint32_t GetTicks();
}
