#pragma once
#include "Types.h"

// STACK (array): history of maintenance changes so the last one can be undone.
class UndoStack {
    static const int CAP = 50;
    Action data[CAP];
    int top = -1;
public:
    bool isEmpty() const { return top == -1; }
    bool isFull() const  { return top == CAP - 1; }

    bool push(const Action& a) {                      // O(1)
        if (isFull()) return false;
        data[++top] = a;
        return true;
    }
    bool pop(Action& out) {                           // O(1)
        if (isEmpty()) return false;
        out = data[top--];
        return true;
    }
    bool peek(Action& out) const {                    // O(1)
        if (isEmpty()) return false;
        out = data[top];
        return true;
    }
};
