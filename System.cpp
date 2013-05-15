////////////////////////////////////////////////////////////////////////////////
// System.cpp
// - Implements timing and input functions.
////////////////////////////////////////////////////////////////////////////////

#include "System.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __AVR__

///////////// Begin embedded system definitions ////////////////

#include <avr/io.h>
#include <avr/interrupt.h>

#include "spipad.h"

#define FOSC 8000000
#define BAUD   57600
#define MYUBRR    16

extern uint8_t __bss_end;
extern uint8_t __stack;

static int uart_putchar(char c, FILE *stream);
static int uart_getchar(FILE *stream);

static void init_fdev(FILE* stream,
                      int (*put_func)(char, FILE*),
                      int (*get_func)(FILE*),
                      int flags) {
    stream->flags = flags;
    stream->put = put_func;
    stream->get = get_func;
    stream->udata = NULL;
}

static FILE mystdout;
static FILE mystdin;

static int uart_putchar(char c, FILE *stream) {
    if (c == '\n')
        uart_putchar('\r', stream);
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;

    return 0;
}

static int uart_getchar(FILE *stream) {
    while( !(UCSR0A & (1<<RXC0)) );
    return(UDR0);
}

// Initializes AVR UART.
static void init_uart() {
    UBRR0H = MYUBRR >> 8;
    UBRR0L = MYUBRR;
    UCSR0A = (1<<U2X0);
    UCSR0B = (1<<TXEN0);
    UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);
    DDRE = (1<<PORTE1);

    stdout = &mystdout;  // Required for printf over UART.
    stderr = &mystdout;  // Required for fprintf(stderr) over UART.
    stdin = &mystdin;    // Required for scanf over UART.
}

// This gets incremented every millisecond by the timer ISR.
static volatile uint32_t ms_counter;

// AVR timer interrupt handler.
ISR(TIMER1_COMPA_vect)
{
    ++ms_counter;
}

// Initializes AVR timer.
static void init_timer() {
   TCCR1B |= (1 << WGM12); // Configure timer 1 for CTC mode
   TIMSK |= (1 << OCIE1A); // Enable CTC interrupt
   sei();                  // Enable global interrupts
   OCR1A   = 125;          // Set CTC compare value to trigger at 1 kHz given
                           // an 8-MHz clock with prescaler of 64.
   TCCR1B |= ((1 << CS11) | (1 << CS10)); // Start timer at Fcpu/64

   ms_counter = 0;        // Reset millisecond counter
}

// Initializes AVR external memory.
static void init_external_memory() {
    MCUCR = (1<<SRE);
    XMCRB = (1<<XMBK) | (1<<XMM0);
    DDRC = 0xff;
    PORTC = 0;
}

static void system_init() {
    init_fdev(&mystdout, uart_putchar, NULL, _FDEV_SETUP_WRITE);
    init_fdev(&mystdin, NULL, uart_getchar, _FDEV_SETUP_READ);
    init_uart();
    init_timer();
    init_external_memory();

    printf("Stack ranges from 0x%x (%u) to 0x%x (%u)\n",
           &__bss_end, &__bss_end, &__stack, &__stack);
}

///////////// End embedded system definitions ////////////////

#else
#include <SDL/SDL.h>
#endif  // defined(__AVR__)

namespace System {

    bool Init() {
#ifdef __AVR__
        system_init();
        spipad_init();
#else
        atexit(SDL_Quit);
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
            fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
            return false;
        }
#endif  // defined(__AVR__)
        return true;
    }

    KeyState GetKeyState() {
        KeyState key_state;
        memset(&key_state, 0, sizeof(key_state));

#ifdef __AVR__
        SpiPadButtons buttons = spipad_read();
        if (buttons.A)
            key_state.quit = 1;
        if (buttons.START)
            key_state.pause = 1;
        if (buttons.B)
            key_state.new_game = 1;
        if (buttons.B)
            key_state.yes = 1;
        if (buttons.A)
            key_state.no = 1;
        if (buttons.UP)
            key_state.up = 1;
        if (buttons.DOWN)
            key_state.down = 1;
        if (buttons.LEFT)
            key_state.left = 1;
        if (buttons.RIGHT)
            key_state.right = 1;
#else
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
#endif  // defined (__AVR__)

        return key_state;
    }

    uint32_t GetTicks() {
#ifdef __AVR__
        return ms_counter;
#else
        return SDL_GetTicks();
#endif
    }

}  // namespace System
