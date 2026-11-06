#pragma once
#include <iostream>
#include <vector>
#include "Types.h"
#include "Utils.h"
#include "Sorting.h"
#include "Searching.h"

// Payment records. Receipt numbers increase over time, so the list is always
// sorted by receipt number (this is what makes binary search possible).
class TransactionLog {
    std::vector<Transaction> records;
    int nextId = 1;
    double sum = 0;
public:
    Transaction add(const std::string& name, bool member, int station,
                    int start, int end, int minutes, double amount) {
        Transaction t{nextId++, name, member, station, start, end, minutes, amount};
        records.push_back(t);
        sum += amount;
        return t;
    }
    const std::vector<Transaction>& all() const { return records; }
    double total() const { return sum; }

    // Sorting (insertion sort): highest payment first
    std::vector<Transaction> sortedByAmount() const {
        std::vector<Transaction> c = records;
        insertionSort(c, [](const Transaction& a, const Transaction& b) { return a.amount > b.amount; });
        return c;
    }
    // Sorting (bubble sort): longest session first
    std::vector<Transaction> sortedByDuration() const {
        std::vector<Transaction> c = records;
        bubbleSort(c, [](const Transaction& a, const Transaction& b) { return a.minutes > b.minutes; });
        return c;
    }

    // Linear search: every record of a customer, ignoring upper/lower case
    std::vector<Transaction> searchByName(const std::string& name) const {
        std::string key = toLower(name);
        auto match = [&](const Transaction& t) { return toLower(t.name) == key; };
        std::vector<Transaction> found;
        int i = linearSearch(records, match);
        while (i != -1) { found.push_back(records[i]); i = linearSearch(records, match, i + 1); }
        return found;
    }

    // Binary search: one record by receipt number
    const Transaction* findById(int id) const {
        int i = binarySearch(records, id, [](const Transaction& t) { return t.txnId; });
        return i == -1 ? nullptr : &records[i];
    }

    static void print(const std::vector<Transaction>& v) {
        if (v.empty()) { std::cout << "  No transactions found.\n"; return; }
        std::cout << "  " << pad("Rcpt", 7) << pad("Customer", 15) << pad("Type", 9) << pad("PC", 5)
                  << pad("Time", 13) << pad("Min", 6) << "Amount\n";
        divider();
        for (const Transaction& t : v)
            std::cout << "  " << pad(zeros(t.txnId, 4), 7) << pad(t.name, 15)
                      << pad(t.member ? "Member" : "Walk-in", 9)
                      << pad(zeros(t.station, 2), 5)
                      << pad(clockStr(t.startTime) + "-" + clockStr(t.endTime), 13)
                      << pad(std::to_string(t.minutes), 6) << money(t.amount) << "\n";
        divider();
        std::cout << "  " << v.size() << " record(s)\n";
    }
};
