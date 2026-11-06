#pragma once
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cctype>
#include <ctime>
#include <algorithm>

const int WIDTH = 66;

// ---------- layout helpers ----------
inline void divider(char c = '-') { std::cout << std::string(WIDTH, c) << "\n"; }

inline std::string center(const std::string& s, int w) {
    if ((int)s.size() >= w) return s.substr(0, w);
    int space = w - (int)s.size(), left = space / 2;
    return std::string(left, ' ') + s + std::string(space - left, ' ');
}

inline void title(const std::string& text) {
    divider('=');
    std::cout << center(text, WIDTH) << "\n";
    divider('=');
}

// left-aligned table column, always leaves at least one trailing space
inline std::string pad(const std::string& s, int w) {
    std::string t = (int)s.size() > w - 1 ? s.substr(0, w - 1) : s;
    return t + std::string(w - (int)t.size(), ' ');
}

inline std::string zeros(int v, int w) {
    std::ostringstream o;
    o << std::setfill('0') << std::setw(w) << v;
    return o.str();
}

inline std::string fixedNum(double v, int prec) {
    std::ostringstream o;
    o << std::fixed << std::setprecision(prec) << v;
    return o.str();
}

inline std::string money(double v) { return "PHP " + fixedNum(v, 2); }

// real wall-clock time as HH:MM
inline std::string clockStr(std::time_t t) {
    std::tm* lt = std::localtime(&t);
    char buf[8];
    std::strftime(buf, sizeof buf, "%H:%M", lt);
    return buf;
}

inline std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return (char)std::tolower(c); });
    return s;
}

// ---------- input helpers ----------
inline int readInt(const std::string& prompt, int lo, int hi) {
    while (true) {
        std::cout << prompt;
        std::string s;
        if (!std::getline(std::cin, s)) return lo;       // input closed
        try {
            size_t pos = 0;
            int v = std::stoi(s, &pos);
            while (pos < s.size() && std::isspace((unsigned char)s[pos])) pos++;
            if (pos == s.size() && v >= lo && v <= hi) return v;
        } catch (...) {}
        std::cout << "  Please enter a number from " << lo << " to " << hi << ".\n";
    }
}

inline std::string readName(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string s;
        if (!std::getline(std::cin, s)) return "Guest";
        size_t a = s.find_first_not_of(" \t\r");
        size_t b = s.find_last_not_of(" \t\r");
        if (a != std::string::npos) return s.substr(a, b - a + 1);
        std::cout << "  Name cannot be empty.\n";
    }
}

inline void waitForEnter() {
    std::cout << "\n  Press Enter to continue...";
    std::string s;
    std::getline(std::cin, s);
}
