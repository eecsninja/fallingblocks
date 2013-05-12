//////////////////////////////////////////////////////////////////////////////////
// StateStack.h
// - A statically allocated stack to store function pointers.
//////////////////////////////////////////////////////////////////////////////////

#pragma once

typedef void (*StatePointer)();

#define STATE_STACK_MAX_SIZE       16

class StateStack {
  private:
    StatePointer pointers[STATE_STACK_MAX_SIZE];
    int stack_size;

  public:
    StateStack() : stack_size(0) {}

    void push(StatePointer ptr);
    StatePointer pop();

    StatePointer top() const {
        return pointers[stack_size - 1];
    }

    bool empty() const {
        return stack_size == 0;
    }
};

//  Aaron Cox, 2004 //
//  Simon Que, 2013 //
