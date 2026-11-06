#pragma once
#include <iostream>
#include "Types.h"
#include "Utils.h"

// QUEUE (linked list): customers waiting for a free computer, first come first served.
class WaitQueue {
    struct Node { Customer data; Node* next; };
    Node *front = nullptr, *rear = nullptr;
    int count = 0;
public:
    ~WaitQueue() { Customer c; while (dequeue(c)) {} }

    void enqueue(const Customer& c) {                 // O(1)
        Node* n = new Node{c, nullptr};
        if (rear) rear->next = n; else front = n;
        rear = n;
        count++;
    }
    bool dequeue(Customer& out) {                     // O(1)
        if (!front) return false;
        Node* t = front;
        out = t->data;
        front = front->next;
        if (!front) rear = nullptr;
        delete t;
        count--;
        return true;
    }
    bool peek(Customer& out) const {                  // O(1)
        if (!front) return false;
        out = front->data;
        return true;
    }
    bool isEmpty() const { return count == 0; }
    int size() const { return count; }

    void display() const {
        if (!front) { std::cout << "  The waiting line is empty.\n"; return; }
        std::cout << "  " << pad("No.", 6) << pad("Customer", 16) << pad("Type", 10) << "Waiting since\n";
        divider();
        int pos = 1;
        for (Node* cur = front; cur; cur = cur->next)
            std::cout << "  " << pad(std::to_string(pos++), 6) << pad(cur->data.name, 16)
                      << pad(cur->data.memberId ? "Member" : "Walk-in", 10)
                      << clockStr(cur->data.arrivedAt) << "\n";
    }
};
