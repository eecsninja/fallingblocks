//////////////////////////////////////////////////////////////////////////////////
// StateStack.h
// - A statically allocated stack to store game states.
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#define STATE_STACK_MAX_SIZE       16

class StateStack {
  private:
    int states[STATE_STACK_MAX_SIZE];
    int stack_size;

  public:
    StateStack() : stack_size(0) {}

    void push(int state);
    int pop();

    int top() const {
        return states[stack_size - 1];
    }

    bool empty() const {
        return stack_size == 0;
    }
};

//  Aaron Cox, 2004 //
//  Simon Que, 2013 //
