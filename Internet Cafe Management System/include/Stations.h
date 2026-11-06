#pragma once
#include <iostream>
#include "Types.h"
#include "Utils.h"

// ARRAY: the fixed list of computers and their status.
class StationManager {
    Computer pcs[NUM_STATIONS];
public:
    StationManager() {
        for (int i = 0; i < NUM_STATIONS; i++) { pcs[i].id = i + 1; pcs[i].status = AVAILABLE; }
    }
    bool isValid(int id) const { return id >= 1 && id <= NUM_STATIONS; }
    Status getStatus(int id) const { return pcs[id - 1].status; }       // O(1)
    void setStatus(int id, Status s) { pcs[id - 1].status = s; }        // O(1)

    int findFree() const {                                              // O(n) scan
        for (int i = 0; i < NUM_STATIONS; i++)
            if (pcs[i].status == AVAILABLE) return pcs[i].id;
        return -1;
    }
    int count(Status s) const {
        int n = 0;
        for (int i = 0; i < NUM_STATIONS; i++) if (pcs[i].status == s) n++;
        return n;
    }

    void displayBoard() const {
        const int W = 13;
        int rows = NUM_STATIONS / GRID_COLS;
        auto border = [&]() {
            std::cout << "  +";
            for (int c = 0; c < GRID_COLS; c++) std::cout << std::string(W, '-') << "+";
            std::cout << "\n";
        };
        for (int r = 0; r < rows; r++) {
            border();
            std::cout << "  |";
            for (int c = 0; c < GRID_COLS; c++)
                std::cout << center("PC " + zeros(pcs[r * GRID_COLS + c].id, 2), W) << "|";
            std::cout << "\n  |";
            for (int c = 0; c < GRID_COLS; c++)
                std::cout << center(statusName(pcs[r * GRID_COLS + c].status), W) << "|";
            std::cout << "\n";
        }
        border();
    }
};
