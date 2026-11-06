#pragma once
#include <iostream>
#include "Types.h"
#include "Utils.h"

// SINGLY LINKED LIST: customers currently using a computer.
class SessionList {
    Session* head = nullptr;
    Session* tail = nullptr;
    int nextId = 1;
public:
    ~SessionList() {
        while (head) { Session* t = head; head = head->next; delete t; }
        tail = nullptr;
    }

    bool hasSessions() const { return head != nullptr; }

    Session* add(const Customer& c, int station, std::time_t startTime) {     // insert at end, O(1)
        Session* node = new Session{};
        node->sessionId = nextId++;
        node->customer  = c;
        node->station   = station;
        node->startTime = startTime;
        node->status    = ACTIVE;
        node->next      = nullptr;
        if (tail) tail->next = node;
        else head = node;
        tail = node;
        return node;
    }

    Session* find(int station) const {                                // traverse, O(n)
        for (Session* cur = head; cur; cur = cur->next)
            if (cur->station == station) return cur;
        return nullptr;
    }

    bool remove(int station, Session& out) {                          // remove, O(n)
        Session *cur = head, *prev = nullptr;
        while (cur && cur->station != station) { prev = cur; cur = cur->next; }
        if (!cur) return false;
        out = *cur;
        if (prev) prev->next = cur->next; else head = cur->next;
        if (tail == cur) tail = prev;
        delete cur;
        return true;
    }

    // minutes of billable time so far (paused time is not billed)
    static int usedMinutes(const Session& s, std::time_t now) {
        long long paused = s.pausedTotal;
        if (s.status == PAUSED) paused += (long long)(now - s.pausedSince);
        long long secs = (long long)(now - s.startTime) - paused;
        if (secs < 0) secs = 0;
        return (int)((secs + 59) / 60);                  // rounded up to the next minute
    }

    void display(std::time_t now) const {
        if (!head) { std::cout << "  No active sessions.\n"; return; }
        std::cout << "  " << pad("ID", 5) << pad("Customer", 15) << pad("Type", 9)
                  << pad("PC", 5) << pad("Start", 8) << pad("Used", 9) << "Status\n";
        divider();
        for (Session* cur = head; cur; cur = cur->next) {
            std::cout << "  " << pad(zeros(cur->sessionId, 3), 5)
                      << pad(cur->customer.name, 15)
                      << pad(cur->customer.memberId ? "Member" : "Walk-in", 9)
                      << pad(zeros(cur->station, 2), 5)
                      << pad(clockStr(cur->startTime), 8)
                      << pad(std::to_string(usedMinutes(*cur, now)) + " min", 9)
                      << (cur->status == PAUSED ? "Paused" : "Active") << "\n";
        }
    }
};
