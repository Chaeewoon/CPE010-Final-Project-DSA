#pragma once
#include <string>

// QUEUE (circular array): receipts waiting for the printer, printed in order.
class ReceiptQueue {
    static const int CAP = 20;
    std::string data[CAP];
    int front = 0, count = 0;
public:
    bool isFull() const  { return count == CAP; }
    bool isEmpty() const { return count == 0; }
    int size() const { return count; }

    bool enqueue(const std::string& s) {              // O(1)
        if (isFull()) return false;
        data[(front + count) % CAP] = s;              // wrap around
        count++;
        return true;
    }
    bool dequeue(std::string& out) {                  // O(1)
        if (isEmpty()) return false;
        out = data[front];
        front = (front + 1) % CAP;
        count--;
        return true;
    }
};
