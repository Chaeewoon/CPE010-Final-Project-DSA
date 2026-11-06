#pragma once
#include <vector>
#include <utility>

// Insertion sort: O(n^2) worst case, O(n) best case
template <typename T, typename Less>
void insertionSort(std::vector<T>& a, Less less) {
    for (int i = 1; i < (int)a.size(); i++) {
        T key = a[i];
        int j = i - 1;
        while (j >= 0 && less(key, a[j])) { a[j + 1] = a[j]; j--; }
        a[j + 1] = key;
    }
}

// Bubble sort with early exit: O(n^2) worst case, O(n) best case
template <typename T, typename Less>
void bubbleSort(std::vector<T>& a, Less less) {
    for (int i = 0; i < (int)a.size() - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < (int)a.size() - 1 - i; j++)
            if (less(a[j + 1], a[j])) { std::swap(a[j], a[j + 1]); swapped = true; }
        if (!swapped) break;
    }
}
