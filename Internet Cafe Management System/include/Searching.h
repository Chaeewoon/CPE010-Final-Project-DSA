#pragma once
#include <vector>

// Linear search: O(n), works on unsorted data
template <typename T, typename Match>
int linearSearch(const std::vector<T>& a, Match match, int start = 0) {
    for (int i = start; i < (int)a.size(); i++)
        if (match(a[i])) return i;
    return -1;
}

// Binary search: O(log n), data must be sorted by the key
template <typename T, typename KeyOf>
int binarySearch(const std::vector<T>& a, int target, KeyOf keyOf) {
    int lo = 0, hi = (int)a.size() - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        int k = keyOf(a[mid]);
        if (k == target) return mid;
        if (k < target) lo = mid + 1; else hi = mid - 1;
    }
    return -1;
}
