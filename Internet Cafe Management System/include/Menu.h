#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "Cafe.h"
#include "Benchmark.h"

// Menu screens and user input.

inline void showMenuItem(int key, const std::string& name, const std::string& note = "") {
    std::cout << "   [" << key << "] " << pad(name, 28) << note << "\n";
}

// ---------------- Front Desk ----------------
inline void checkInAction(InternetCafe& cafe) {
    cafe.showBoard();
    std::cout << "\n";
    std::string name = readName("  Customer name: ");
    int memberId = readInt("  Member ID (0 if walk-in): ", 0, 999999);
    int pref = readInt("  Preferred PC, 0 for any: ", 0, NUM_STATIONS);
    std::cout << "\n";
    cafe.checkIn(name, memberId, pref);
}

inline void groupAction(InternetCafe& cafe) {
    cafe.showBoard();
    std::cout << "\n";
    int size = readInt("  Group size (2-" + std::to_string(NUM_STATIONS) + "): ", 2, NUM_STATIONS);
    std::vector<std::string> names;
    for (int i = 1; i <= size; i++) names.push_back(readName("  Name of person " + std::to_string(i) + ": "));
    std::cout << "\n";
    if (!cafe.checkInGroup(names)) {
        if (readInt("  Add the group to the waiting line? (1 = yes, 0 = no): ", 0, 1) == 1)
            cafe.queueGroup(names);
    }
}

inline void endSessionAction(InternetCafe& cafe) {
    cafe.showSessions();
    if (!cafe.hasSessions()) return;
    int pc = readInt("\n  PC number to end (0 = cancel): ", 0, NUM_STATIONS);
    std::cout << "\n";
    if (pc != 0) cafe.endSession(pc);
}

inline void pauseAction(InternetCafe& cafe) {
    cafe.showSessions();
    if (!cafe.hasSessions()) return;
    int pc = readInt("\n  PC number to pause or resume (0 = cancel): ", 0, NUM_STATIONS);
    std::cout << "\n";
    if (pc != 0) cafe.togglePause(pc);
}

inline void frontDesk(InternetCafe& cafe) {
    int choice;
    do {
        std::cout << "\n";
        title("FRONT DESK");
        showMenuItem(1, "Check in a customer");
        showMenuItem(2, "Check in a group", "seats them together");
        showMenuItem(3, "End session and pay");
        showMenuItem(4, "Pause / resume a session");
        showMenuItem(5, "Active sessions");
        showMenuItem(6, "Waiting line");
        showMenuItem(0, "Back");
        divider();
        choice = readInt("  Select: ", 0, 6);
        if (choice == 0) break;
        std::cout << "\n";
        switch (choice) {
            case 1: checkInAction(cafe);      break;
            case 2: groupAction(cafe);        break;
            case 3: endSessionAction(cafe);   break;
            case 4: pauseAction(cafe);        break;
            case 5: cafe.showSessions();      break;
            case 6: cafe.showQueue();         break;
        }
        waitForEnter();
    } while (true);
}

// ---------------- Computers ----------------
inline void computersMenu(InternetCafe& cafe) {
    int choice;
    do {
        std::cout << "\n";
        title("COMPUTERS");
        showMenuItem(1, "Status board");
        showMenuItem(2, "Maintenance on / off");
        showMenuItem(3, "Undo last maintenance change");
        showMenuItem(0, "Back");
        divider();
        choice = readInt("  Select: ", 0, 3);
        if (choice == 0) break;
        std::cout << "\n";
        if (choice == 1) cafe.showBoard();
        else if (choice == 2) {
            cafe.showBoard();
            int pc = readInt("\n  PC number (0 = cancel): ", 0, NUM_STATIONS);
            std::cout << "\n";
            if (pc != 0) cafe.toggleMaintenance(pc);
        } else cafe.undoLast();
        waitForEnter();
    } while (true);
}

// ---------------- Members ----------------
inline void membersMenu(InternetCafe& cafe) {
    int choice;
    do {
        std::cout << "\n";
        title("MEMBERS");
        showMenuItem(1, "Register a new member");
        showMenuItem(2, "Look up a member by ID");
        showMenuItem(3, "Member list");
        showMenuItem(0, "Back");
        divider();
        choice = readInt("  Select: ", 0, 3);
        if (choice == 0) break;
        std::cout << "\n";
        if (choice == 1) cafe.registerMember(readName("  Member name: "));
        else if (choice == 2) cafe.showMember(readInt("  Member ID: ", 0, 999999));
        else cafe.listMembers();
        waitForEnter();
    } while (true);
}

// ---------------- Billing and reports ----------------
inline void billingMenu(InternetCafe& cafe) {
    int choice;
    do {
        std::cout << "\n";
        title("BILLING AND REPORTS");
        showMenuItem(1, "All transactions");
        showMenuItem(2, "Highest payments first");
        showMenuItem(3, "Longest sessions first");
        showMenuItem(4, "Search by customer name");
        showMenuItem(5, "Find a receipt by number");
        showMenuItem(6, "Print next receipt");
        showMenuItem(7, "Daily sales summary");
        showMenuItem(0, "Back");
        divider();
        choice = readInt("  Select: ", 0, 7);
        if (choice == 0) break;
        std::cout << "\n";
        switch (choice) {
            case 1: TransactionLog::print(cafe.log().all());               break;
            case 2: TransactionLog::print(cafe.log().sortedByAmount());    break;
            case 3: TransactionLog::print(cafe.log().sortedByDuration());  break;
            case 4: {
                std::string name = readName("  Customer name: ");
                std::cout << "\n";
                TransactionLog::print(cafe.log().searchByName(name));
                break;
            }
            case 5: {
                int id = readInt("  Receipt number: ", 1, 999999);
                std::cout << "\n";
                const Transaction* t = cafe.log().findById(id);
                if (t) TransactionLog::print({*t});
                else std::cout << "  Receipt " << zeros(id, 4) << " was not found.\n";
                break;
            }
            case 6: cafe.printNextReceipt(); break;
            case 7: cafe.salesSummary();     break;
        }
        waitForEnter();
    } while (true);
}

// ---------------- Main menu ----------------
inline void mainMenu(InternetCafe& cafe) {
    int choice;
    do {
        std::cout << "\n";
        title("NETCAFE MANAGEMENT SYSTEM");
        cafe.showDashboard();
        divider();
        showMenuItem(1, "Front Desk",         "check in, end session, queue");
        showMenuItem(2, "Computers",          "status board, maintenance");
        showMenuItem(3, "Members",            "register, look up, list");
        showMenuItem(4, "Billing and Reports","history, sort, search, receipts");
        showMenuItem(5, "Performance Lab",    "time complexity tests");
        showMenuItem(0, "Exit");
        divider();
        choice = readInt("  Select: ", 0, 5);
        switch (choice) {
            case 1: frontDesk(cafe);     break;
            case 2: computersMenu(cafe); break;
            case 3: membersMenu(cafe);   break;
            case 4: billingMenu(cafe);   break;
            case 5: std::cout << "\n"; runAnalysis(); waitForEnter(); break;
            case 0: std::cout << "\n  Goodbye.\n"; break;
        }
    } while (choice != 0);
}
