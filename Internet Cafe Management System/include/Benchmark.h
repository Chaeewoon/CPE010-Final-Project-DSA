#pragma once
#include <iostream>
#include <chrono>
#include <vector>
#include "Utils.h"
#include "Sorting.h"
#include "Searching.h"

// Performance Lab: theoretical complexity table + measured running times.
inline void runAnalysis() {
    using namespace std::chrono;

    title("PERFORMANCE LAB: TIME COMPLEXITY");
    std::cout << "  THEORETICAL (n = number of items, V = PCs, E = links)\n";
    divider();
    std::cout << "  " << pad("Operation", 32) << pad("Structure", 20) << "Time\n";
    divider();
    const char* rows[][3] = {
        {"Look up a PC by number",         "Array",              "O(1)"},
        {"Find a free PC",                 "Array scan",         "O(n)"},
        {"Start / end a session",          "Singly linked list", "O(n)"},
        {"Join / leave the waiting line",  "Linked queue",       "O(1)"},
        {"Queue / print a receipt",        "Circular queue",     "O(1)"},
        {"Record / undo a PC change",      "Array stack",        "O(1)"},
        {"Find / register a member",       "AVL tree",           "O(log n)"},
        {"Nearest PC / seat a group",      "Graph BFS / DFS",    "O(V + E) / O(V^2)"},
        {"Sort payments",                  "Insertion, bubble",  "O(n^2)"},
        {"Search payments by name",        "Linear search",      "O(n)"},
        {"Find a receipt by number",       "Binary search",      "O(log n)"},
    };
    for (auto& r : rows) std::cout << "  " << pad(r[0], 32) << pad(r[1], 20) << r[2] << "\n";

    // ---- searching ----
    std::cout << "\n  MEASURED: searching (worst case, nanoseconds per search)\n";
    divider();
    std::cout << "  " << pad("n", 11) << pad("Linear", 14) << pad("Growth", 10)
              << pad("Binary", 14) << "Growth\n";
    divider();
    auto ident = [](int x) { return x; };
    double prevL = 0, prevB = 0;
    for (int n : {10000, 100000, 1000000}) {
        std::vector<int> a(n);
        for (int i = 0; i < n; i++) a[i] = i;
        const int RL = 200, RB = 200000;
        volatile int sink = 0;
        auto t0 = steady_clock::now();
        for (int r = 0; r < RL; r++) { int target = -1 - r; sink = linearSearch(a, [target](int x) { return x == target; }); }
        auto t1 = steady_clock::now();
        for (int r = 0; r < RB; r++) sink = binarySearch(a, -1 - r, ident);
        auto t2 = steady_clock::now();
        (void)sink;
        double lin = duration<double, std::nano>(t1 - t0).count() / RL;
        double bin = duration<double, std::nano>(t2 - t1).count() / RB;
        std::cout << "  " << pad(std::to_string(n), 11)
                  << pad(fixedNum(lin, 0), 14) << pad(prevL > 0 ? "x" + fixedNum(lin / prevL, 1) : "-", 10)
                  << pad(fixedNum(bin, 1), 14) << (prevB > 0 ? "x" + fixedNum(bin / prevB, 1) : "-") << "\n";
        prevL = lin; prevB = bin;
    }
    std::cout << "  n grows 10x per row: linear time grows about 10x, binary barely moves.\n";

    // ---- sorting ----
    std::cout << "\n  MEASURED: sorting (worst case, reverse order, microseconds)\n";
    divider();
    std::cout << "  " << pad("n", 11) << pad("Insertion", 14) << pad("Growth", 10)
              << pad("Bubble", 14) << "Growth\n";
    divider();
    auto less = [](int a, int b) { return a < b; };
    double prevI = 0, prevBu = 0;
    for (int n : {1000, 2000, 4000, 8000}) {
        std::vector<int> a(n);
        for (int i = 0; i < n; i++) a[i] = n - i;
        std::vector<int> b = a;
        auto t0 = steady_clock::now();
        insertionSort(a, less);
        auto t1 = steady_clock::now();
        bubbleSort(b, less);
        auto t2 = steady_clock::now();
        double ins = duration<double, std::micro>(t1 - t0).count();
        double bub = duration<double, std::micro>(t2 - t1).count();
        std::cout << "  " << pad(std::to_string(n), 11)
                  << pad(fixedNum(ins, 0), 14) << pad(prevI > 0 ? "x" + fixedNum(ins / prevI, 1) : "-", 10)
                  << pad(fixedNum(bub, 0), 14) << (prevBu > 0 ? "x" + fixedNum(bub / prevBu, 1) : "-") << "\n";
        prevI = ins; prevBu = bub;
    }
    std::cout << "  n doubles per row: both sorts grow about 4x, which matches O(n^2).\n";
}
