#pragma once
#include <string>
#include <ctime>

const int    NUM_STATIONS     = 10;
const int    GRID_COLS        = 5;          // floor layout: 2 rows x 5 columns
const double RATE_PER_HOUR    = 20.0;       // PHP per hour, billed per minute
const double MEMBER_DISCOUNT  = 0.10;       // 10% off for members

enum Status        { AVAILABLE, OCCUPIED, MAINTENANCE };
enum SessionStatus { ACTIVE, PAUSED };

inline std::string statusName(Status s) {
    switch (s) {
        case AVAILABLE: return "FREE";
        case OCCUPIED:  return "IN USE";
        default:        return "MAINTENANCE";
    }
}

struct Computer { int id; Status status; };

struct Customer {
    int id;
    std::string name;
    int memberId;          // 0 means walk-in
    std::time_t arrivedAt; // real time of arrival
};

struct Session {
    int sessionId;
    Customer customer;
    int station;
    std::time_t startTime;
    SessionStatus status;
    std::time_t pausedSince;   // when the current pause began
    long long pausedTotal;     // seconds spent paused so far
    Session* next;
};

struct Transaction {
    int txnId;
    std::string name;
    bool member;
    int station;
    std::time_t startTime;
    std::time_t endTime;
    int minutes;           // billed minutes
    double amount;         // final amount paid
};

struct Member { int id; std::string name; int visits; double totalSpent; };

struct Action { int station; Status previous; };
