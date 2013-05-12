//////////////////////////////////////////////////////////////////////////////////
// StateStack.cpp
// - Implements functions from StateStack class.
//////////////////////////////////////////////////////////////////////////////////

#include "StateStack.h"

#include <stdio.h>

void StateStack::push(StatePointer ptr) {
    if (stack_size == STATE_STACK_MAX_SIZE) {
        fprintf(stderr, "Attempting to push to full stack!\n");
        return;
    }
    pointers[stack_size++] = ptr;
}

StatePointer StateStack::pop() {
    if (empty()) {
        fprintf(stderr, "Attempting to pop from empty stack!\n");
        return NULL;
    }
    return pointers[--stack_size];
}

//  Aaron Cox, 2004 //
//  Simon Que, 2013 //
