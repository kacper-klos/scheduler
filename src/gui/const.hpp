#pragma once

#include <QStringList>

constexpr uint8_t kWeekDaysSize = 7;
const QStringList kWeekDays = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

template <typename T1, typename T2> struct SemiMutablePair {
    T1 first;
    mutable T2 second;
    // Constructor
    constexpr SemiMutablePair() : first(), second() {}
    constexpr SemiMutablePair(const T1 &a, const T2 &b) : first(a), second(b) {}
    // Comparator
    constexpr bool operator<(const SemiMutablePair &other) const { return (first < other.first); }
};
