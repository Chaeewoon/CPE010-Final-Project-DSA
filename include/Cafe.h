#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Types.h"
#include "Utils.h"
#include "Stations.h"
#include "SessionList.h"
#include "WaitQueue.h"
#include "ReceiptQueue.h"
#include "UndoStack.h"
#include "TransactionLog.h"
#include "MemberTree.h"
#include "FloorGraph.h"

// Handles cafe operations and coordinates the data structures.
class InternetCafe {
    StationManager stations;     // array
    SessionList    sessions;     // singly linked list
    WaitQueue      waiting;      // queue (linked list)
    ReceiptQueue   receipts;     // queue (circular array)
    UndoStack      undoStack;    // stack (array)
    TransactionLog txnLog;       // sorting + searching
    MemberTree     members;      // AVL tree
    FloorGraph     floorPlan;    // graph, BFS, DFS

    int nextCustomerId = 1;
    int nextMemberId   = 1001;

    static std::time_t now() { return std::time(nullptr); }   // real time
    static double computeFee(int minutes) { return minutes * RATE_PER_HOUR / 60.0; }
    static std::string tag(const Customer& c) { return c.name + (c.memberId != 0 ? " (Member)" : ""); }

    void startSession(const Customer& c, int pc) {
        stations.setStatus(pc, OCCUPIED);
        sessions.add(c, pc, now());
        std::cout << "  " << tag(c) << " is seated at PC " << zeros(pc, 2) << ".\n";
    }

    // Assign free stations to customers in waiting order.
    void serveWaiting() {
        while (!waiting.isEmpty()) {
            int pc = stations.findFree();
            if (pc == -1) break;
            Customer c;
            waiting.dequeue(c);
            std::cout << "  Next in line: ";
            startSession(c, pc);
        }
    }

    std::string buildReceipt(const Transaction& t, double subtotal, double discount) const {
        std::ostringstream o;
        std::string bar = "  " + std::string(40, '=');
        o << bar << "\n  " << center("NETCAFE RECEIPT", 40) << "\n" << bar << "\n";
        o << "  Receipt No : " << zeros(t.txnId, 4) << "\n";
        o << "  Customer   : " << t.name << (t.member ? " (Member)" : "") << "\n";
        o << "  Station    : PC " << zeros(t.station, 2) << "\n";
        o << "  Time       : " << clockStr(t.startTime) << " - " << clockStr(t.endTime) << "\n";
        o << "  Billed     : " << t.minutes << " min\n";
        o << "  Rate       : " << money(RATE_PER_HOUR) << " / hour\n";
        o << "  Subtotal   : " << money(subtotal) << "\n";
        if (t.member)
            o << "  Discount   : -" << money(discount) << " (" << (int)(MEMBER_DISCOUNT * 100) << "%)\n";
        o << "  TOTAL      : " << money(t.amount) << "\n" << bar << "\n";
        return o.str();
    }

public:
    TransactionLog& log() { return txnLog; }
    bool hasSessions() const { return sessions.hasSessions(); }

    // ---------- dashboard ----------
    void showDashboard() const {
        std::cout << "  Time: " << clockStr(now()) << "          Sales today: " << money(txnLog.total()) << "\n";
        std::cout << "  PCs : " << stations.count(AVAILABLE) << " free | " << stations.count(OCCUPIED)
                  << " in use | " << stations.count(MAINTENANCE) << " maintenance\n";
        std::cout << "  Waiting line: " << waiting.size() << " | Receipts to print: " << receipts.size() << "\n";
    }

    // ---------- front desk ----------
    void showBoard() const { stations.displayBoard(); }
    void showSessions() const { sessions.display(now()); }
    void showQueue() const { waiting.display(); }

    void checkIn(const std::string& name, int memberId, int preferred) {
        Customer c{nextCustomerId++, name, memberId, now()};
        if (memberId != 0 && !members.find(memberId)) {
            std::cout << "  Member ID " << memberId << " was not found. Checked in as a walk-in.\n";
            c.memberId = 0;
        }
        if (stations.findFree() == -1) {
            waiting.enqueue(c);
            std::cout << "  All PCs are busy. " << c.name << " joined the waiting line at position "
                      << waiting.size() << ".\n";
            return;
        }
        int pc;
        if (preferred == 0) {
            pc = stations.findFree();
        } else if (stations.getStatus(preferred) == AVAILABLE) {
            pc = preferred;
        } else {
            int dist = 0;
            pc = floorPlan.nearestFree(preferred, stations, dist);
            std::cout << "  PC " << zeros(preferred, 2) << " is not available. Closest free PC is "
                      << zeros(pc, 2) << " (" << dist << (dist == 1 ? " seat" : " seats") << " away).\n";
        }
        startSession(c, pc);
    }

    bool checkInGroup(const std::vector<std::string>& names) {
        std::vector<int> pcs;
        if (!floorPlan.findGroup((int)names.size(), stations, pcs)) {
            std::cout << "  No block of " << names.size() << " connected free PCs right now.\n";
            return false;
        }
        for (size_t i = 0; i < names.size(); i++) {
            Customer c{nextCustomerId++, names[i], 0, now()};
            startSession(c, pcs[i]);
        }
        return true;
    }

    void queueGroup(const std::vector<std::string>& names) {
        for (const std::string& n : names) {
            waiting.enqueue(Customer{nextCustomerId++, n, 0, now()});
            std::cout << "  " << n << " joined the waiting line at position " << waiting.size() << ".\n";
        }
    }

    void togglePause(int pc) {
        Session* s = sessions.find(pc);
        if (!s) { std::cout << "  PC " << zeros(pc, 2) << " has no active session.\n"; return; }
        if (s->status == ACTIVE) {
            s->status = PAUSED;
            s->pausedSince = now();
            std::cout << "  " << s->customer.name << "'s session is paused. Billing is stopped.\n";
        } else {
            s->pausedTotal += (long long)(now() - s->pausedSince);
            s->status = ACTIVE;
            std::cout << "  " << s->customer.name << "'s session is resumed. Billing continues.\n";
        }
    }

    void endSession(int pc) {
        if (!sessions.find(pc)) { std::cout << "  PC " << zeros(pc, 2) << " has no active session.\n"; return; }
        Session s{};
        int minutes = SessionList::usedMinutes(*sessions.find(pc), now());
        if (minutes < 1) minutes = 1;
        sessions.remove(pc, s);

        bool isMember = s.customer.memberId != 0;
        double subtotal = computeFee(minutes);
        double discount = isMember ? subtotal * MEMBER_DISCOUNT : 0.0;
        double total = subtotal - discount;

        Transaction t = txnLog.add(s.customer.name, isMember, pc, s.startTime, now(), minutes, total);
        std::string receipt = buildReceipt(t, subtotal, discount);

        if (isMember) {
            Member* m = members.find(s.customer.memberId);
            if (m) { m->visits++; m->totalSpent += total; }
        }
        stations.setStatus(pc, AVAILABLE);

        std::cout << "  Session ended for " << tag(s.customer) << " on PC " << zeros(pc, 2) << ".\n";
        std::cout << "  Billed time : " << minutes << " min\n";
        std::cout << "  Total due   : " << money(total) << "\n";
        if (receipts.enqueue(receipt)) {
            std::cout << "  Receipt " << zeros(t.txnId, 4) << " was added to the print queue.\n";
        } else {
            std::cout << "  The print queue is full, so the receipt is shown here:\n\n" << receipt;
        }
        serveWaiting();
    }

    // ---------- computers ----------
    void toggleMaintenance(int pc) {
        Status cur = stations.getStatus(pc);
        if (cur == OCCUPIED) { std::cout << "  PC " << zeros(pc, 2) << " is in use. End its session first.\n"; return; }
        if (!undoStack.push({pc, cur})) { std::cout << "  The change history is full.\n"; return; }
        Status next = (cur == AVAILABLE) ? MAINTENANCE : AVAILABLE;
        stations.setStatus(pc, next);
        std::cout << "  PC " << zeros(pc, 2) << " is now " << statusName(next) << ".\n";
        if (next == AVAILABLE) serveWaiting();
    }

    void undoLast() {
        Action a{};
        if (!undoStack.peek(a)) { std::cout << "  There is nothing to undo.\n"; return; }
        if (stations.getStatus(a.station) == OCCUPIED) {
            std::cout << "  Cannot undo: PC " << zeros(a.station, 2) << " is in use now.\n";
            return;
        }
        undoStack.pop(a);
        stations.setStatus(a.station, a.previous);
        std::cout << "  Undone. PC " << zeros(a.station, 2) << " is back to " << statusName(a.previous) << ".\n";
        if (a.previous == AVAILABLE) serveWaiting();
    }

    // ---------- members ----------
    void registerMember(const std::string& name) {
        int id = nextMemberId++;
        members.insert(Member{id, name, 0, 0.0});
        std::cout << "  " << name << " is now a member. Member ID: " << id << "\n";
        std::cout << "  Members get " << (int)(MEMBER_DISCOUNT * 100) << "% off every session.\n";
    }

    void showMember(int id) {
        Member* m = members.find(id);
        if (!m) { std::cout << "  No member found with ID " << id << ".\n"; return; }
        std::cout << "  Member ID   : " << m->id << "\n  Name        : " << m->name
                  << "\n  Visits      : " << m->visits << "\n  Total spent : " << money(m->totalSpent) << "\n";
    }

    void listMembers() const {
        std::vector<Member> all = members.listAll();
        if (all.empty()) { std::cout << "  No members registered yet.\n"; return; }
        std::cout << "  " << pad("ID", 8) << pad("Name", 20) << pad("Visits", 9) << "Total spent\n";
        divider();
        for (const Member& m : all)
            std::cout << "  " << pad(std::to_string(m.id), 8) << pad(m.name, 20)
                      << pad(std::to_string(m.visits), 9) << money(m.totalSpent) << "\n";
        divider();
        std::cout << "  " << all.size() << " member(s)\n";
    }

    // ---------- billing ----------
    void printNextReceipt() {
        std::string r;
        if (receipts.dequeue(r)) std::cout << r << "  " << receipts.size() << " receipt(s) still waiting.\n";
        else std::cout << "  No receipts are waiting to be printed.\n";
    }

    void salesSummary() const {
        const std::vector<Transaction>& all = txnLog.all();
        if (all.empty()) { std::cout << "  No sales yet today.\n"; return; }
        int minutes = 0, memberCount = 0;
        for (const Transaction& t : all) { minutes += t.minutes; if (t.member) memberCount++; }
        std::cout << "  Customers served : " << all.size() << " (" << memberCount << " members, "
                  << all.size() - memberCount << " walk-ins)\n";
        std::cout << "  Total time billed: " << minutes << " min\n";
        std::cout << "  Total sales      : " << money(txnLog.total()) << "\n";
        std::cout << "  Average per customer: " << money(txnLog.total() / all.size()) << "\n";
    }
};
